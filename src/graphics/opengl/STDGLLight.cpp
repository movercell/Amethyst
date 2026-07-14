#include "STDGLLight.h"
#include <numbers>

void STDGLLight::Update() {
    if (wasChangedProjection) {
        Info.Projection = glm::perspective(glm::radians(FOV), Resolution.x / Resolution.y, Far, Near); // Because reverse-Z.
    }
    if (wasChanged || wasChangedProjection) {
        Info.View = glm::lookAt(Position.toglm(), (Position + Front).toglm(), Up.toglm());
        Info.ViewProjection = Info.Projection * Info.View;

        Info.InverseView = glm::inverse(Info.View.toglm());
        Info.InverseViewProjection = glm::inverse(Info.ViewProjection.toglm());

        Info.Frustum = Shapes::Frustum(Info.ViewProjection);
        Info.CameraPos = Position;
        glNamedBufferSubData(Infobuffer, 0, sizeof(STDGLCamera::Camerainfo_t), &Info);

        UpdateData();

        wasChanged = false;
        wasChangedProjection = false;
    }
}

void STDGLLight::Bind() {
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, Infobuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
    glViewport(TextureSpace.PosX, TextureSpace.PosY, TextureSpace.SizeX, TextureSpace.SizeY);
}

void STDGLLight::SetPosition(vec3 position) {
    Position = position;

    wasChanged = true;
}
void STDGLLight::SetRotation(quat rotation)  {
    mat4 RotationMatrix = quat(rotation).MakeRotationMatrix();

    Front = RotationMatrix[0].ToVec3();
    Left = RotationMatrix[1].ToVec3();
    Up = RotationMatrix[2].ToVec3();

    wasChanged = true;
}
void STDGLLight::SetColor(vec3 color) {
    Color = color;

    wasChanged = true;
}
void STDGLLight::SetInnerAngle(float Inner) {
    InnerCutoffCosine = cos(Inner * (std::numbers::pi / 180.0));

    wasChanged = true;
}
void STDGLLight::SetOuterAngle(float Outer) {
    InnerCutoffCosine = cos(Outer * (std::numbers::pi / 180.0));
    FOV = Outer * 2.0f;

    wasChanged = true;
    wasChangedProjection = true;
}
void STDGLLight::SetFalloff(float Constant, float Linear, float Quadraric) {
    Falloff = vec3(Constant, Linear, Quadraric);

    wasChanged = true;
}
void STDGLLight::SetIntensity(float intensity) {
    Intensity = intensity;

    wasChanged = true;
}

void STDGLLight::CreateBuffers() {
    glCreateFramebuffers(1, &Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);

    glNamedFramebufferTexture(Framebuffer, GL_DEPTH_ATTACHMENT, Owner->LightDepthBuffer, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // Information buffer
    glCreateBuffers(1, &Infobuffer);
    glNamedBufferStorage(Infobuffer, sizeof(STDGLCamera::Camerainfo_t), nullptr, GL_DYNAMIC_STORAGE_BIT);
}

void STDGLLight::UpdateData() {
    STDGLLightData Data;
    Data.View = Info.View;
    Data.Projection = Info.Projection;
    Data.Type = Type;
    Data.Color = Color;
    Data.FOV = FOV;
    Data.Spot_InnerCutoff = InnerCutoffCosine;
    Data.Spot_OuterCutoff = OuterCutoffCosine;
    Data.Spot_Direction = Front;
    Data.Near = Near;
    Data.Far = Far;
    Data.Falloff_Constant = Falloff.x;
    Data.Falloff_Linear = Falloff.y;
    Data.Falloff_Quadratic = Falloff.z;
    Data.Intensity = Intensity;
    Data.TextureSpace = TextureSpace;
    Data.LightPos = Position;
    glNamedBufferSubData(Owner->LightDataBuffer, sizeof(STDGLLightData) * ID, sizeof(STDGLLightData), &Data);
}


STDGLLight::STDGLLight(STDGLLightSystem* owner, Engine::Reference<RWorld> rworldref, uint32_t id, STDGLLightType type, vec2 resolution, float inner_cutoff_angle, float outer_cutoff_angle, vec3 color, float near, float far) {
    Owner = owner;
    RWorldRef = rworldref;
    ID = id;
    Type = type;
    Resolution = resolution;
    // Don't forget to pad the block to prevent texture bleeding artifacts! 
    TextureSpace = Owner->LightAreaAllocator.AllocPadded(resolution.x, resolution.y, STDGLLIGHT_ALLOC2D_PADDING);
    FOV = outer_cutoff_angle * 2.0f;
    InnerCutoffCosine = cos(inner_cutoff_angle * (std::numbers::pi / 180.0));
    OuterCutoffCosine = cos(outer_cutoff_angle * (std::numbers::pi / 180.0));

    Color = color;
    Near = near;
    Far = far;

    CreateBuffers();
}

STDGLLight::~STDGLLight() {
    glfwMakeContextCurrent(Owner->Context);

    glDeleteFramebuffers(1, &Framebuffer);
    glDeleteBuffers(1, &Infobuffer);

    // The block has padding that needs to be accounted for when freeing.
    Owner->LightAreaAllocator.FreePadded(TextureSpace, STDGLLIGHT_ALLOC2D_PADDING);
}




Engine::Reference<Light> STDGLLightSystem::MakeLight(Engine::Reference<RWorld> RWorldRef, STDGLLightType Type, vec2 resolution, float inner_cutoff_angle, float outer_cutoff_angle, vec3 color, float near, float far) {
    uint32_t ID;
    if (FreedIndices.empty()) {
        ID = NextIndexToMake++;
    } else {
        ID = FreedIndices.front();
        FreedIndices.pop();
    }

    auto* Resource = new Engine::ManagedInterfacedResource<STDGLLightSystem, Light, STDGLLight>(this, this, RWorldRef, ID, Type, resolution, inner_cutoff_angle, outer_cutoff_angle, color, near, far);
    LightResources[ID] = Resource;
    return Engine::Reference(Resource);
}

STDGLLightSystem::STDGLLightSystem() {
    Context = glfwGetCurrentContext();
    LightResources.fill(nullptr);

    glCreateBuffers(1, &LightDataBuffer);

    auto* DefaultData = new STDGLLightData[STDGLLIGHT_MAX_COUNT];
    glNamedBufferData(LightDataBuffer, sizeof(STDGLLightData) * STDGLLIGHT_MAX_COUNT, DefaultData, GL_STATIC_DRAW);
    delete DefaultData;

    // In case of sparse textures being available.
    if (GLAD_GL_ARB_sparse_texture) {
        // Get the maximum aligments.
        GLint SparseAlignmentX;
        {
            GLint SparseAlignmentXA;
            glGetInternalformativ(GL_TEXTURE_2D, DepthFormat, GL_VIRTUAL_PAGE_SIZE_X_ARB, 1, &SparseAlignmentXA);
            GLint SparseAlignmentXB;
            glGetInternalformativ(GL_TEXTURE_2D, MomentFormat, GL_VIRTUAL_PAGE_SIZE_X_ARB, 1, &SparseAlignmentXB);
            SparseAlignmentX = std::max(SparseAlignmentXA, SparseAlignmentXB);
        }
        GLint SparseAlignmentY;
        {
            GLint SparseAlignmentYA;
            glGetInternalformativ(GL_TEXTURE_2D, DepthFormat, GL_VIRTUAL_PAGE_SIZE_Y_ARB, 1, &SparseAlignmentYA);
            GLint SparseAlignmentYB;
            glGetInternalformativ(GL_TEXTURE_2D, MomentFormat, GL_VIRTUAL_PAGE_SIZE_Y_ARB, 1, &SparseAlignmentYB);
            SparseAlignmentY = std::max(SparseAlignmentYA, SparseAlignmentYB);
        }

        LightAreaAllocator = Geometry::Alloc2D(STDGLLIGHT_ALLOC2D_DIMENSTIONS, STDGLLIGHT_ALLOC2D_DIMENSTIONS, SparseAlignmentX, SparseAlignmentY);

        LightAreaAllocator.SetCallbacks(
            [this](Geometry::Alloc2D::Block block) -> void {
                glfwMakeContextCurrent(Context);
                if (isLightDepthBufferSparse) {
                    glTexturePageCommitmentEXT(LightDepthBuffer, 0, block.PosX, block.PosY, 0, block.SizeX, block.SizeY, 1, GL_TRUE);
                    glClearTexSubImage(LightDepthBuffer, 0, block.PosX, block.PosY, 0, block.SizeX, block.SizeY, 1, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
                }
            },
            [this](Geometry::Alloc2D::Block block) -> void {
                glfwMakeContextCurrent(Context);
                if (isLightDepthBufferSparse) {
                    glTexturePageCommitmentEXT(LightDepthBuffer, 0, block.PosX, block.PosY, 0, block.SizeX, block.SizeY, 1, GL_FALSE);
                }
            });

        GLint VirtualPageSizesForDepth;
        glGetInternalformativ(GL_TEXTURE_2D, DepthFormat, GL_NUM_VIRTUAL_PAGE_SIZES_ARB, 1, &VirtualPageSizesForDepth);
        
        if (VirtualPageSizesForDepth != -1) {
            isLightDepthBufferSparse = true;
        }

        isLightMomentBufferSparse = true;

        Engine::Warning("Sparse!");
    }

    // Depth buffer
    glCreateTextures(GL_TEXTURE_2D, 1, &LightDepthBuffer);
    if (isLightDepthBufferSparse) glTextureParameteri(LightDepthBuffer, GL_TEXTURE_SPARSE_ARB, GL_TRUE);
    glTextureStorage2D (LightDepthBuffer, 1, DepthFormat, STDGLLIGHT_ALLOC2D_DIMENSTIONS, STDGLLIGHT_ALLOC2D_DIMENSTIONS);
    glTextureParameteri(LightDepthBuffer, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(LightDepthBuffer, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(LightDepthBuffer, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTextureParameteri(LightDepthBuffer, GL_TEXTURE_COMPARE_FUNC, GL_GEQUAL);

    float clearColor = 0.0f;
    glClearTexImage(LightDepthBuffer, 0, GL_DEPTH_COMPONENT, GL_FLOAT, &clearColor);
}

STDGLLightSystem::~STDGLLightSystem() {
    glDeleteBuffers(1, &LightDataBuffer);
    glDeleteTextures(1, &LightDepthBuffer);
}