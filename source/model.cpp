#include"../headers/model.h"

Model::Model()
{   
}

void Model::Load(const char* file, rp3d::PhysicsCommon& physicsCommon)
{   
    std::string text = getFileContents(file);
    JSON = json::parse(text);

    fileStr = std::string(file);
    fileDirectory = fileStr.substr(0,fileStr.find_last_of('/')+1);

    Model::file = file;
    data = loadData();
    materials = loadMaterials();
    textures = loadTextures();

    colliders = loadColliders(physicsCommon);
    for(auto &collider : colliders){
        collisionShapes.push_back(collider.shape);
    }

    traverseNode(0);
}

std::vector<Collider> Model::loadColliders(rp3d::PhysicsCommon& physicsCommon)
{
    std::vector<Collider> colliders;
    // Open colliders object.
    std::string line;
    std::ifstream colliderFile (fileDirectory+"collider.obj");
    // Checks file exists.
    if(!colliderFile)
    {
        return colliders;
    }

    while(getline(colliderFile, line))
    {
        if(line.empty()) continue;
        if(line.at(0) == '#') continue;
        std::vector<std::string> words = split(line, ' ');
        if(words.empty()) continue;
        if(words[0] == "o")
        {
            colliders.push_back(Collider{});
        }
        else if(words[0] == "v")
        {
            colliders.back().numVertices++;
        }
        else if(words[0] == "f")
        {            
            colliders.back().numFaces++;
            colliders.back().numIndices += words.size()-1;
        }
    }

    for(auto &collider : colliders){
        collider.vertices = new float[collider.numVertices*3];
        collider.indices = new int[collider.numIndices];
        collider.faces = new rp3d::PolygonVertexArray::PolygonFace[collider.numFaces]; 
    }

    colliderFile.clear();
    colliderFile.seekg(0);

    int n = -1;
    int vertIndex = 0;
    int indIndex = 0;
    int faceInd = 0;
    int indOffset = 0;

    while(getline(colliderFile, line))
    {
        std::cout << line << std::endl;
        if(line.empty()) continue;
        if(line.at(0) == '#') continue;
        std::vector<std::string> words = split(line, ' ');
        if(words.empty()) continue;

        if(words[0] == "o")
        {
            n++;
            vertIndex = 0;
            indIndex = 0;
            faceInd = 0;

            if(n > 0)
            indOffset += colliders[n-1].numVertices;
        }
        else if(words[0] == "v")
        {
            colliders[n].vertices[vertIndex++] = std::stof(words[1]);
            colliders[n].vertices[vertIndex++] = std::stof(words[2]);
            colliders[n].vertices[vertIndex++] = std::stof(words[3]);
        }
        else if(words[0] == "f")
        {            
            colliders[n].faces[faceInd].indexBase = indIndex;
            colliders[n].faces[faceInd].nbVertices = words.size()-1;
            faceInd++;

            for(int i = 1; i < words.size(); i++){
                std::vector<std::string> indexStrings = split(words[i], '/');

                if(indexStrings.empty())
                throw std::runtime_error("Model could not parse face - invalid face indexes.");

                if(!isNumber(indexStrings[0]))
                throw std::runtime_error("Model could not parse face - index not a number.");

                int index = (std::stoi(indexStrings[0]) -1) - indOffset;
                if(index < 0)

                throw std::runtime_error("Model could not parse index, index < 0.");

                colliders[n].indices[indIndex++] = index;
            }
        }
    }
    colliderFile.close();

    for (auto &collider : colliders){        
        std::cout << "num vertices " << collider.numVertices << std::endl;
        std::cout << "num faces " << collider.numFaces << std::endl;
        std::cout << "num ind " << collider.numIndices << std::endl;

        for(int i = 0; i < collider.numIndices; i++){
            std::cout << "ind " << collider.indices[i] << std::endl;
        }
        for(int i = 0; i < collider.numVertices; i++){
            std::cout << "vert " << collider.vertices[i] << std::endl;
        }
    }
    std::cout << "done " << std::endl;

    for (auto &collider : colliders){
        // Create the polygon vertex array 
        collider.polygonArray = new rp3d::PolygonVertexArray(collider.numVertices, collider.vertices, 3 * sizeof(float), 
        collider.indices, sizeof(int), collider.numFaces, collider.faces, 
        rp3d::PolygonVertexArray::VertexDataType::VERTEX_FLOAT_TYPE, 
        rp3d::PolygonVertexArray::IndexDataType::INDEX_INTEGER_TYPE); 
        
        // Create the polyhedron mesh 
        collider.mesh = physicsCommon.createPolyhedronMesh(collider.polygonArray); 
        
        // Create the convex mesh collision shape 
        collider.shape = physicsCommon.createConvexMeshShape(collider.mesh);
    }

    return colliders;
}

std::vector<rp3d::ConvexMeshShape*>& Model::getCollisionShapes(){
    return collisionShapes;
}

void Model::setMass(rp3d::decimal mass){
    Model::mass = mass;
}
rp3d::decimal Model::getMass(){
    return mass;
}
void Model::setModelPosition(glm::vec3 position)
{
    Model::modelPosition = position;
    updateLocal();
}
void Model::setModelOrientation(glm::vec3 orientation, glm::vec3 up)
{
    if (up == glm::vec3(0.0f,0.0f,0.0f))
    {
        up = glm::normalize(glm::cross(orientation, glm::vec3(0.0f,1.0f,0.0f)));
    }
    glm::mat4 rotMatrix = glm::lookAt(glm::vec3(0), orientation, glm::normalize(up));
    Model::modelQuaternion = glm::quat_cast(rotMatrix);
    updateLocal();
}
void Model::setModelScale(glm::vec3 scale)
{
    Model::modelScale = scale;
    updateLocal();

    rp3d::Vector3 sca(scale.x, scale.y, scale.z);
    for(auto &collider : colliders){
        collider.shape->setScale(sca);
    }
}
void Model::updateLocal(){
    glm::mat4 trans = glm::mat4(1.0f);
    glm::mat4 rot = glm::mat4(1.0f);
    glm::mat4 sca = glm::mat4(1.0f);

    trans = glm::translate(trans, modelPosition);
    rot = glm::toMat4(modelQuaternion);
    sca = glm::scale(sca, modelScale);

    modelTransform = trans * rot * sca;
}

void Model::setTransform(glm::mat4& transform){
    Model::transform = transform;
}

void Model::prepareInstance(glm::mat4& transform){
    glm::mat4 matrix = transform * modelTransform;
    if(instanceIndex >= instanceMatrices.size()){
        instanceMatrices.push_back(matrix);
    }else{
        instanceMatrices[instanceIndex] = matrix;
    }
    instanceIndex++;
}

bool Model::readyToInstance(){
    return numInstances >= instancingThreshold;
}

void Model::drawInstanced(Shader& instancedShader, Camera& camera)
{  
    instanceIndex = 0;
    instancedShader.Activate();
    camera.sendMatrix(instancedShader, "camMatrix");
    camera.sendPosition(instancedShader, "camPos");
    for (unsigned int i = 0; i < meshes.size(); i++){
        meshes[i].setInstanceMatrices(instanceMatrices);
        meshes[i].drawInstanced(instancedShader, camera, textures, numInstances);
    }
}
void Model::drawBatch(Shader& shader, Camera& camera)
{  
    instanceIndex = 0;
    shader.Activate();
    camera.sendMatrix(shader, "camMatrix");
    camera.sendPosition(shader, "camPos");
    for(int i = 0; i < numInstances; i++){
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "transform"), 1, GL_FALSE, glm::value_ptr(instanceMatrices[i]));
        for (int j = 0; j < meshes.size(); j++){
            meshes[j].Mesh::Draw(shader, camera, textures);
        }
    }
} 

void Model::Draw(Shader& shader, Camera& camera)
{
    setUniforms(shader, camera);
    for (unsigned int i = 0; i < meshes.size(); i++){
        meshes[i].Mesh::Draw(shader, camera, textures);
    }
} 

void Model::drawPrepOutline(Shader& blankShader, Camera& camera)
{
    setUniforms(blankShader, camera);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    glDisable(GL_DEPTH_TEST);
    glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
    for (unsigned int i = 0; i < meshes.size(); i++){
        meshes[i].Mesh::drawSimple(blankShader, camera);
    }
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glStencilMask(0x00);
}

void Model::drawId(Shader& shader, Camera& camera, int id){
    setUniforms(shader, camera);
    glUniform1i(glGetUniformLocation(shader.ID, "objectID"), id);
    for (unsigned int i = 0; i < meshes.size(); i++){
        meshes[i].Mesh::drawSimple(shader, camera);
    }
}

void Model::drawOutline(Shader& outlineShader, Camera& camera){
    setUniforms(outlineShader, camera);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glUniform1f(glGetUniformLocation(outlineShader.ID, "outlineThickness"), 0.05f);
    for (unsigned int i = 0; i < meshes.size(); i++){
        meshes[i].Mesh::drawSimple(outlineShader, camera);
    }
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
}

void Model::setUniforms(Shader& shader, Camera& camera){
    shader.Activate();
    camera.sendMatrix(shader, "camMatrix");
    camera.sendPosition(shader, "camPos");
    glm::mat4 matrix = transform * modelTransform;
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "transform"), 1, GL_FALSE, glm::value_ptr(matrix));
}

std::vector<unsigned char> Model::loadData()
{
    std::cout << "in loadData" << std::endl;
    std::string bytesText;
    std::string uri = JSON["buffers"][0]["uri"];

    std::cout << "1" << std::endl;

    std::cout << fileDirectory.c_str() << std::endl;
    std::cout << (fileDirectory + uri).c_str() << std::endl;
    bytesText = getFileContents((fileDirectory + uri).c_str());

    std::cout << "2" << std::endl;

    std::vector<unsigned char> data(bytesText.begin(), bytesText.end());

    std::cout << "3" << std::endl;

    return data;
}

std::vector<float> Model::getFloats(json accessor)
{
    std::vector<float> floatVec;

    unsigned int buffViewInd = accessor.value("bufferView",1);
    unsigned int count = accessor["count"];
    unsigned int accByteOffset = accessor.value("byteOffset",0);
    std::string type = accessor["type"];

    json bufferView = JSON["bufferViews"][buffViewInd];
    unsigned int byteOffset = bufferView.value("byteOffset", 0);

    unsigned int numPerVert;
    if (type == "SCALAR") numPerVert = 1;
    else if (type == "VEC2") numPerVert = 2;
    else if (type == "VEC3") numPerVert = 3;
    else if (type == "VEC4") numPerVert = 4;
    else throw std::invalid_argument("Type is invalid (not SCALAR, VEC2, VEC3 or VEC4)");

    unsigned int beginningOfData = byteOffset + accByteOffset;
    unsigned int lengthOfData = count * 4 * numPerVert;

    for (unsigned int i = beginningOfData; i < beginningOfData + lengthOfData; i+=4){
        unsigned char bytes[] = {data[i], data[i+1], data[i+2], data[i+3]};
        float value;
        std::memcpy(&value, bytes, sizeof(float));
        floatVec.push_back(value);
    }

    return floatVec;
}

std::vector<GLuint> Model::getIndices(json accessor)
{
    std::cout << "in getIndices" << std::endl;

    std::vector<GLuint> indices;

	unsigned int buffViewInd = accessor.value("bufferView", 0);     std::cout << "buffViewInd" << std::endl;
	unsigned int count = accessor["count"];                         std::cout << "count" << std::endl;
	unsigned int accByteOffset = accessor.value("byteOffset", 0);   std::cout << "accByteOffset" << std::endl;
	unsigned int componentType = accessor["componentType"];         std::cout << "componentType" << std::endl;

	json bufferView = JSON["bufferViews"][buffViewInd]; std::cout << "bufferView" << std::endl;

    std::cout << bufferView.dump() << std::endl;
	unsigned int byteOffset = bufferView.value("byteOffset", 0); std::cout << "byteOffset" << std::endl;

    std::cout << "in" << std::endl;

    unsigned int beginningOfData = byteOffset + accByteOffset;
	if (componentType == 5125)
	{
		for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 4; i+=4)
		{
            unsigned char bytes[] = {data[i], data[i+1], data[i+2], data[i+3]};
			unsigned int value;
			std::memcpy(&value, bytes, sizeof(unsigned int));
			indices.push_back((GLuint)value);
		}
	}
	else if (componentType == 5123)
	{
		for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i+=2)
		{
			unsigned char bytes[] = {data[i], data[i+1]};
			unsigned short value;
			std::memcpy(&value, bytes, sizeof(unsigned short));
			indices.push_back((GLuint)value);
		}
	}
	else if (componentType == 5122)
	{
		for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i+=2)
		{
			unsigned char bytes[] = {data[i], data[i+1]};
			short value;
			std::memcpy(&value, bytes, sizeof(short));
			indices.push_back((GLuint)value);
		}
	}

    std::cout << "out" << std::endl;

    return indices;
}

std::vector<Vertex> Model::assembleVertices(
    std::vector<glm::vec3> positions,
    std::vector<glm::vec3> normals,
    std::vector<glm::vec2> texUVs)
{
    std::vector<Vertex> vertices;
    for (int i = 0; i < positions.size(); i++){
        vertices.push_back(
            Vertex{
                positions[i],
                glm::normalize(normals[i]),
                glm::vec3(1.0f,1.0f,1.0f),
                texUVs[i]
            }
            );
    }
    return vertices;
}

std::vector<struct Material> Model::loadMaterials()
{
    /* This is an example of the material json format we are dealing with...

  "materials": [
    {
      "doubleSided": true,
      "name": "material",
      "pbrMetallicRoughness": {
        "baseColorTexture": {
          "index": 0
        },
        "metallicFactor": 0.08075229982549903,
        "metallicRoughnessTexture": {
          "index": 1
        },
        "roughnessFactor": 0.5428454602387076
      }
    },
*/

    std::vector<struct Material> materials;

    // loop through materials in model json
    // and save as vector of material structs
    for (unsigned int i = 0; i < JSON["materials"].size(); i++){
        std::string     name                = JSON["materials"][i].value("name", std::string("unnamed"));
        bool            doubleSided         = JSON["materials"][i].value("doubleSided", true);

        // pbrMetallicRoughness is optional, but almost all materials will have it,
        // it contains the most important things, like the indexes of textures
        bool hasPbrMetRough = JSON["materials"][i].contains("pbrMetallicRoughness");

        json            pbrMetRough;
        glm::vec4       baseColorFactor;
        float           metallicFactor;
        float           roughnessFactor;
        bool            hasBaseColorTex;
        int             baseColorTexture;
        int             metallicRoughnessTexture;

        if( hasPbrMetRough )
        {
            pbrMetRough = JSON["materials"][i]["pbrMetallicRoughness"];
            if (pbrMetRough.contains("baseColorFactor")){
                json baseColorFactorJson = pbrMetRough["baseColorFactor"];
                if (baseColorFactorJson.size() != 4){
                    throw std::runtime_error("Size of baseColorFactorJson != 4");
                }
                // not especially elegant conversion
                baseColorFactor = glm::vec4(
                    baseColorFactorJson.at(0),
                    baseColorFactorJson.at(1),
                    baseColorFactorJson.at(2),
                    baseColorFactorJson.at(3)
                );
            }
            else
            {
                baseColorFactor = glm::vec4(1.0f,1.0f,1.0f,1.0f);
            }
            std::cout << "between here" << std::endl;    
            metallicFactor      = pbrMetRough.value("metallicFactor", 1.0f);
            roughnessFactor     = pbrMetRough.value("roughnessFactor", 1.0f);

            if (pbrMetRough.contains("baseColorTexture")){
                baseColorTexture = pbrMetRough["baseColorTexture"]["index"];
                hasBaseColorTex = true;
            }else{
                baseColorTexture = -1;
                hasBaseColorTex = false;
            }
            if (pbrMetRough.contains("metallicRoughnessTexture")){
                metallicRoughnessTexture = pbrMetRough["metallicRoughnessTexture"]["index"];
            }else{
                metallicRoughnessTexture = -1;
            }
        
            std::cout << "and here" << std::endl;
        }
        else
        {
            baseColorFactor     = glm::vec4(1.0f,1.0f,1.0f,1.0f);
            metallicFactor      = 1.0f;
            roughnessFactor     = 1.0f;
            hasBaseColorTex          = false;
            baseColorTexture         = -1;
            metallicRoughnessTexture = -1;
        }

        struct Material material{
            name,               //std::string     name                = std::string("Initial string");
            doubleSided,        //bool            doubleSided         = true;
            baseColorFactor,    //glm::vec4       baseColorFactor     = glm::vec4(1.0f,1.0f,1.0f,1.0f);
            metallicFactor,     //float           metallicFactor      = 1.0f;
            roughnessFactor,    //float           roughnessFactor     = 1.0f;
            hasBaseColorTex,            //bool            hasBaseColorTex          = false;
            baseColorTexture,           //int             baseColorTexture         = -1;
            metallicRoughnessTexture    //int             metallicRoughnessTexture = -1;
        };

        materials.push_back(material);
    }

    return materials;
}

std::vector<Texture> Model::loadTextures()
{
    
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < JSON["textures"].size(); i++){

        int texInd = JSON["textures"][i]["source"];

        std::string texPath = JSON["images"][texInd]["uri"];

        bool skip = false;
        for (unsigned int j = 0; j < loadedTexName.size(); j++)
        {
            if (loadedTexName[j] == texPath)
            {
                textures.push_back(textures[j]);
                skip = true;
                break;
            }
        }

        if(!skip)
        {
            Texture texture = Texture((fileDirectory+texPath).c_str(), "something", i);
            textures.push_back(texture);
            loadedTexName.push_back(texPath);  
        }
    }
    return textures;
}

void Model::loadMesh(unsigned int indMesh, glm::mat4 matrix)
{
    std::cout << "loadMesh" << indMesh << std::endl;

    unsigned int posAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["POSITION"];
    unsigned int normalAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["NORMAL"];
    unsigned int texAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["TEXCOORD_0"];
    unsigned int indAccInd = JSON["meshes"][indMesh]["primitives"][0]["indices"];

    unsigned int matInd = JSON["meshes"][indMesh]["primitives"][0]["material"];

    std::vector<float> posVec = getFloats(JSON["accessors"][posAccInd]);
    std::vector<glm::vec3> positions = groupFloatsVec3(posVec);

    std::vector<float> normalVec = getFloats(JSON["accessors"][normalAccInd]);
    std::vector<glm::vec3> normals = groupFloatsVec3(normalVec);

    std::vector<float> texVec = getFloats(JSON["accessors"][texAccInd]);
    std::vector<glm::vec2> texUVs = groupFloatsVec2(texVec);

    std::vector<Vertex> vertices = assembleVertices(positions, normals, texUVs);
    std::vector<GLuint> indices = getIndices(JSON["accessors"][indAccInd]);

    struct Material material = materials[matInd];

    meshes.push_back(Mesh(vertices, indices, material, matrix));
}

void Model::traverseNode(unsigned int nextNode, glm::mat4 matrix){
    std::cout << "nodes" << std::endl;
    json node = JSON["nodes"][nextNode];

    std::cout << "translation" << std::endl;

	// Get translation if it exists
	glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
	if (node.find("translation") != node.end())
	{
		float transValues[3];
		for (unsigned int i = 0; i < node["translation"].size(); i++)
			transValues[i] = (node["translation"][i]);
		translation = glm::make_vec3(transValues);
	}
    std::cout << "rotation" << std::endl;
	// Get quaternion if it exists
	glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	if (node.find("rotation") != node.end())
	{
		float rotValues[4] =
		{
			node["rotation"][3],
			node["rotation"][0],
			node["rotation"][1],
			node["rotation"][2]
		};
		rotation = glm::make_quat(rotValues);
	}
    std::cout << "scale" << std::endl;
	// Get scale if it exists
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	if (node.find("scale") != node.end())
	{
		float scaleValues[3];
		for (unsigned int i = 0; i < node["scale"].size(); i++)
			scaleValues[i] = (node["scale"][i]);
		scale = glm::make_vec3(scaleValues);
	}
    std::cout << "matrix" << std::endl;
	// Get matrix if it exists
	glm::mat4 matNode = glm::mat4(1.0f);
	if (node.find("matrix") != node.end())
	{
		float matValues[16];
		for (unsigned int i = 0; i < node["matrix"].size(); i++)
			matValues[i] = (node["matrix"][i]);
		matNode = glm::make_mat4(matValues);
	}
    std::cout << "final" << std::endl;

    glm::mat4 trans = glm::mat4(1.0f);
    glm::mat4 rot = glm::mat4(1.0f);
    glm::mat4 sca = glm::mat4(1.0f);

    trans = glm::translate(trans, translation);
    rot = glm::mat4_cast(rotation);
    sca = glm::scale(sca, scale);

    glm::mat4 matNextNode = matrix * matNode * trans * rot * sca;

    std::cout << "mesh" << std::endl;

	if (node.find("mesh") != node.end())
	{
        loadMesh(node["mesh"], matrix);
    }

    std::cout << "children" << std::endl;

	if (node.find("children") != node.end())
	{
        for (unsigned int i = 0; i < node["children"].size(); i++)
        {
            traverseNode(node["children"][i], matNextNode);
        }
    }

    std::cout << "end" << std::endl;
}

std::vector<glm::vec2> Model::groupFloatsVec2(std::vector<float> floatVec)
{
	std::vector<glm::vec2> vectors;
	for (int i = 0; i < floatVec.size(); i+=2)
	{
		vectors.push_back(glm::vec2(floatVec[i], floatVec[i+1]));
	}
	return vectors;
}
std::vector<glm::vec3> Model::groupFloatsVec3(std::vector<float> floatVec)
{
	std::vector<glm::vec3> vectors;
	for (int i = 0; i < floatVec.size(); i+=3)
	{
		vectors.push_back(glm::vec3(floatVec[i], floatVec[i+1], floatVec[i+2]));
	}
	return vectors;
}
std::vector<glm::vec4> Model::groupFloatsVec4(std::vector<float> floatVec)
{
	std::vector<glm::vec4> vectors;
	for (int i = 0; i < floatVec.size(); i+=4)
	{
		vectors.push_back(glm::vec4(floatVec[i], floatVec[i+1], floatVec[i+2], floatVec[i+3]));
	}
	return vectors;
}

void Model::addInstance(){
    numInstances++;
}
void Model::delInstance(){
    numInstances--;
}