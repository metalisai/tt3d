#include "shader.h"

#include <fstream>
#include <sstream>

GLuint createShader(GLenum eShaderType, const std::string &strShaderFile)
{
    GLuint shader = glCreateShader(eShaderType);
    const char *strFileData = strShaderFile.c_str();
    glShaderSource(shader, 1, &strFileData, NULL);

    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

        const char *strShaderType = NULL;
        switch(eShaderType)
        {
        case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
        case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
        case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
        }

        fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
        delete[] strInfoLog;
    }

    return shader;
}

GLuint loadShader(GLenum eShaderType, const std::string &strShaderFilename)
{
    std::string strFilename = strShaderFilename;
    std::ifstream shaderFile(strFilename.c_str());
    std::stringstream shaderData;
    shaderData << shaderFile.rdbuf();
    shaderFile.close();

    try
    {
        return createShader(eShaderType, shaderData.str());
    }
    catch(std::exception &e)
    {
        fprintf(stderr, "%s\n", e.what());
        throw;
    }
}

GLuint createProgram(const std::vector<GLuint> &shaderList)
{
    GLuint program = glCreateProgram();

    for(size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
        glAttachShader(program, shaderList[iLoop]);

    glLinkProgram(program);

    GLint status;
    glGetProgramiv (program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
        fprintf(stderr, "Linker failure: %s\n", strInfoLog);
        delete[] strInfoLog;
    }

    for(size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
        glDetachShader(program, shaderList[iLoop]);

    return program;
}

void initializeComputeProgram(Shader* shader, const char* computeFile, ShaderType type)
{
    std::vector<GLuint> shaderList;
    shader->type = type;
    shaderList.push_back(loadShader(GL_COMPUTE_SHADER, computeFile));
    //shaderList.push_back(LoadShader(GL_FRAGMENT_SHADER, fragFile));
    shader->program = createProgram(shaderList);
    for (std::vector<GLuint>::iterator it = shaderList.begin() ; it != shaderList.end(); ++it)
    {
        glDeleteShader(*it);
    }
    switch(type)
    {
    case ST_LightCull:
        printf("Compute shader loaded!\n");
        shader->lightc.depthMap    = glGetUniformLocation(shader->program, "depthMap");
        shader->lightc.lightCount  = glGetUniformLocation(shader->program, "lightCount");
        shader->lightc.projection  = glGetUniformLocation(shader->program, "projection");
        shader->lightc.screenSize  = glGetUniformLocation(shader->program, "screenSize");
        shader->lightc.view        = glGetUniformLocation(shader->program, "view");
        if(shader->lightc.depthMap == 0xFFFFFFFF)
            printf("depthMap location not found  %s\n"   ,computeFile);
        else if(shader->lightc.lightCount == 0xFFFFFFFF)
            printf("lightCount location not found  %s\n" ,computeFile);
        else if(shader->lightc.projection == 0xFFFFFFFF)
            printf("projection location not found  %s\n" ,computeFile);
        else if(shader->lightc.screenSize == 0xFFFFFFFF)
            printf("screenSize location not found  %s\n" ,computeFile);
        else if(shader->lightc.view == 0xFFFFFFFF)
            printf("view location not found  %s\n"       ,computeFile);
        break;
    }
}

void initializeProgram(Shader* shader, const char* vertFile, const char* fragFile, ShaderType type)
{
    std::vector<GLuint> shaderList;
    shader->type = type;
    shaderList.push_back(loadShader(GL_VERTEX_SHADER, vertFile));
    shaderList.push_back(loadShader(GL_FRAGMENT_SHADER, fragFile));
    shader->program = createProgram(shaderList);
    for (std::vector<GLuint>::iterator it = shaderList.begin() ; it != shaderList.end(); ++it)
    {
        glDeleteShader(*it);
    }
    switch(type)
    {
    case ST_Surface:
        shader->surface.lightDirUnif = glGetUniformLocation(shader->program, "lightDir");
        shader->surface.transformMatrixUnif = glGetUniformLocation(shader->program, "transformMatrix");
        shader->surface.perspectiveMatrixUnif = glGetUniformLocation(shader->program, "perspectiveMatrix");
        shader->surface.diffuseTexture = glGetUniformLocation(shader->program, "tex");
        shader->surface.normalTexture = glGetUniformLocation(shader->program, "normalSampler");
        shader->surface.viewMatixUnif = glGetUniformLocation(shader->program, "viewMat");
        /*if(shader->surface.lightDirUnif == 0xFFFFFFFF)
            printf("lightdir location not found  %s %s\n",vertFile,fragFile);
        if(shader->surface.perspectiveMatrixUnif == 0xFFFFFFFF)
            printf("pmatrix location not found %s %s\n",vertFile,fragFile);
        if(shader->surface.transformMatrixUnif == 0xFFFFFFFF)
            printf("tmatrix location not found %s %s\n",vertFile,fragFile);
        if(shader->surface.diffuseTexture == 0xFFFFFFFF)
            printf("diffuseTexture location not found %s %s\n",vertFile,fragFile);
        if(shader->surface.normalTexture == 0xFFFFFFFF)
            printf("normalTexture location not found %s %s\n",vertFile,fragFile);*/
        break;
    case ST_PostProc:
        shader->postproc.albedo_unif = glGetUniformLocation(shader->program, "fbo_texture");
        shader->postproc.depth_unif = glGetUniformLocation(shader->program, "depth_texture");
        shader->postproc.normal_unif = glGetUniformLocation(shader->program, "normal_texture");
        shader->postproc.position_unif = glGetUniformLocation(shader->program, "position_texture");
        shader->postproc.shadow_unif = glGetUniformLocation(shader->program, "shadow_texture");
        shader->postproc.light_unif = glGetUniformLocation(shader->program, "lightDir");
        shader->postproc.projM_unif = glGetUniformLocation(shader->program, "projMatrix");
        shader->postproc.lightProjM_unif = glGetUniformLocation(shader->program, "lightProjectionMatrix");
        /*if(shader->postproc.light_unif == 0xFFFFFFFF)
            printf("lightDir location not found %s %s\n",vertFile,fragFile);
        if(shader->postproc.projM_unif == 0xFFFFFFFF)
            printf("projM_unif location not found %s %s\n",vertFile,fragFile);*/
        if(shader->postproc.lightProjM_unif == 0xFFFFFFFF)
                    printf("lightProjM_unif location not found %s %s\n",vertFile,fragFile);
        break;
    case ST_Skydome:
        shader->skydome.scaleMatrix           = glGetUniformLocation(shader->program, "scaleMatrix"         );
        shader->skydome.perspectiveMatrix     = glGetUniformLocation(shader->program, "perspectiveMatrix"   );
        shader->skydome.transformMatrix       = glGetUniformLocation(shader->program, "transformMatrix"     );
        shader->skydome.v3CameraPos           = glGetUniformLocation(shader->program, "v3CameraPos"         );
        shader->skydome.v3LightDir  		  = glGetUniformLocation(shader->program, "v3LightDir"  		);
        shader->skydome.v3InvWavelength	      = glGetUniformLocation(shader->program, "v3InvWavelength"	    );
        shader->skydome.fCameraHeight	      = glGetUniformLocation(shader->program, "fCameraHeight"	    );
        shader->skydome.fCameraHeight2 	      = glGetUniformLocation(shader->program, "fCameraHeight2"	    );
        shader->skydome.fOuterRadius  	      = glGetUniformLocation(shader->program, "fOuterRadius" 	    );
        shader->skydome.fOuterRadius2  	      = glGetUniformLocation(shader->program, "fOuterRadius2"  	    );
        shader->skydome.fInnerRadius   	      = glGetUniformLocation(shader->program, "fInnerRadius"  	    );
        shader->skydome.fInnerRadius2  	      = glGetUniformLocation(shader->program, "fInnerRadius2"  	    );
        shader->skydome.fKrESun        	      = glGetUniformLocation(shader->program, "fKrESun"       	    );
        shader->skydome.fKmESun         	  = glGetUniformLocation(shader->program, "fKmESun"         	);
        shader->skydome.fKr4PI          	  = glGetUniformLocation(shader->program, "fKr4PI"         	    );
        shader->skydome.fKm4PI         	      = glGetUniformLocation(shader->program, "fKm4PI" 	            );
        shader->skydome.fScale       	      = glGetUniformLocation(shader->program, "fScale"              );
        shader->skydome.fScaleOverScaleDepth  = glGetUniformLocation(shader->program, "fScaleOverScaleDepth");

        /*if(shader->skydome.scaleMatrix      == 0xFFFFFFFF)
            printf("scaleMatrix location not found %s %s\n",vertFile,fragFile);
        if(shader->skydome.perspectiveMatrix      == 0xFFFFFFFF)
            printf("perspectiveMatric location not found %s %s\n",vertFile,fragFile);
        if(shader->skydome.transformMatrix      == 0xFFFFFFFF)
            printf("transformMatric location not found %s %s\n",vertFile,fragFile);
        if(shader->skydome.v3CameraPos          == 0xFFFFFFFF)
            printf("v3CameraPos location not found %s %s\n",vertFile,fragFile);
        if(shader->skydome.v3LightDir  		 == 0xFFFFFFFF)
            printf("v3lightDir location not found %s %s\n",vertFile,fragFile);
        if(shader->skydome.v3InvWavelength	     == 0xFFFFFFFF)
            printf("v3InvWavelenght location not found %s %s\n",vertFile,fragFile);
        if(shader->skydome.fCameraHeight	     == 0xFFFFFFFF)
            printf("fCameraHeight location not found %s %s\n",vertFile,fragFile);
        if(shader->skydome.fCameraHeight2 	     == 0xFFFFFFFF)
            printf("fCameraHeight2 location not found %s %s\n",vertFile,fragFile);
        if(shader->skydome.fOuterRadius  	     == 0xFFFFFFFF)
            printf("fOuterRadius location not found %s %s\n",vertFile,fragFile);
        if(shader->skydome.fOuterRadius2  	     == 0xFFFFFFFF)
            printf("fOuterRadius2 location not found %s %s\n",vertFile,fragFile);
        if(shader->skydome.fInnerRadius   	     == 0xFFFFFFFF)
            printf("fInnerRadius location not found %s %s\n",vertFile,fragFile);
        if(shader->skydome.fInnerRadius2  	     == 0xFFFFFFFF)
            printf("dInnerRadius2 location not found %s %s\n",vertFile,fragFile);
        if(shader->skydome.fKrESun        	     == 0xFFFFFFFF)
            printf("fKrESun location not found %s %s\n",vertFile,fragFile);
        if(shader->skydome.fKmESun         	 == 0xFFFFFFFF)
            printf("fKmESun location not found %s %s\n",vertFile,fragFile);
        if(shader->skydome.fKr4PI          	 == 0xFFFFFFFF)
            printf("fKr4PI location not found %s %s\n",vertFile,fragFile);
        if(shader->skydome.fKm4PI         	     == 0xFFFFFFFF)
            printf("fKm4PI location not found %s %s\n",vertFile,fragFile);
        if(shader->skydome.fScale       	     == 0xFFFFFFFF)
            printf("fScale location not found %s %s\n",vertFile,fragFile);
        if(shader->skydome.fScaleOverScaleDepth == 0xFFFFFFFF)
            printf("fScaleOverScaleDepth location not found %s %s\n",vertFile,fragFile);*/
        break;
    }
}

void deleteProgram(Shader* shader)
{
    glDeleteProgram(shader->program);
}
