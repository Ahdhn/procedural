#include "SquareNode.h"


SquareNode::SquareNode(Shader* generationShader, Shader* renderingShader, FBO* fbo, Vector3<float> position, float size, short numDivisions){
	
	m_ptrTerrainGenerationShader = generationShader;
	//m_ptrTerrainRenderingShader = renderingShader;
	m_ptrFBO = fbo;
	m_ptrTerrainRenderingShader = new Shader("../../ProcTerrain/Shaders/terrainRendering.vert", "../../ProcTerrain/Shaders/terrainRendering.frag");
	//glUniform4fARB(m_ptrTerrainRenderingShader->m_locColor,1, 0, 0, 1);

	m_position = position;
	m_size = size;
	m_numDivisions = numDivisions;
	m_face = new Square(m_position, m_size, m_numDivisions);
	m_gridIndex = -1; //center of the grid (currentNode)
	m_firstTime = true;

	for(int i=0; i<8; i++){
		m_ptrNeighbours[i] = NULL;
	}


	GenerateHeightMap();
	
}

SquareNode::~SquareNode(){
	delete m_face;


}

void SquareNode::GenerateHeightMap(){
	m_ptrFBO = new FBO(512, 512);
	Square* square = new Square(m_position, m_size, 1);


	m_ptrFBO->Enable();
	m_ptrTerrainGenerationShader->Enable();
	
	
	square->Render();
	

	m_ptrTerrainGenerationShader->Disable();
	m_ptrFBO->Disable();

	//aux
	
	float* aux;
	glBindTexture(GL_TEXTURE_2D, m_ptrFBO->m_textureId);
	aux = (float*)malloc(512*512*sizeof(float)*4);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, aux);
	//glReadPixels(0,0,512,512,GL_RGBA,GL_FLOAT,aux);
	
}


void SquareNode::Render(){
	
	//Call the parent
	//Node::Render();
	//TODO: make sure they render only once
	for(int i=0; i<8; i++){
		if(m_ptrNeighbours[i] != NULL)
			m_ptrNeighbours[i]->Render();
	}


	

	m_ptrTerrainRenderingShader->Enable();

	
	//glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_ptrFBO->m_textureId);
	//TODO: do it only once, after generating the heightmap
	glUniform1i(m_ptrTerrainRenderingShader->m_locTexture, 0);
	

	m_face->Render();

	m_ptrTerrainRenderingShader->Disable();
}

bool SquareNode::IsWithin(Vector3<float> position){
	if(position.GetX()  >= m_position.GetX() && position.GetX() <= m_position.GetX() + m_size)
		if(position.GetY()  >= m_position.GetY() && position.GetY() <= m_position.GetY() + m_size)
			return true;


	return false;

}

/*
Left
|   Right
|   |
2 4 7 -> TOP
1 c 6
0 3 5 -> BOTTOM
*/
SquareNode* SquareNode::GetNewStandingNode(Vector3<float> position){
	
	//Right
	if(position.GetX() >= m_position.GetX() + m_size){
		
		return m_ptrNeighbours[6];
	}
	
	//Top
	if(position.GetY() >= m_position.GetY() + m_size){
		
		return m_ptrNeighbours[4];
	}

	//Bottom
	if(position.GetY() <= m_position.GetY()){
		
		return m_ptrNeighbours[3];
	}

	//Left
	if(position.GetX() <= m_position.GetX()){
		
		return m_ptrNeighbours[1];
	}
}

void SquareNode::GenerateNeighbours(SquareNode* m_oldNode, short numNeighbours){
	
	//TODO: Re-point the nodes from the oldNode to the new currentNode, and delete the unused ones
	/*
	if(m_gridIndex == 1){
		m_ptrNeighbours[3] = m_oldNode->m_ptrNeighbours[0];
		m_ptrNeighbours[4] = m_oldNode->m_ptrNeighbours[2];
		m_ptrNeighbours[5] = m_oldNode->m_ptrNeighbours[3];
		m_ptrNeighbours[6] = m_oldNode;
		m_ptrNeighbours[7] = m_oldNode->m_ptrNeighbours[4];
		//nodes to delete: 7, 6, 5
		delete m_oldNode->m_ptrNeighbours[7];
		delete m_oldNode->m_ptrNeighbours[6];
		delete m_oldNode->m_ptrNeighbours[5];
	}
	else if(m_gridIndex == 3){
		m_ptrNeighbours[2] = m_oldNode->m_ptrNeighbours[1];
		m_ptrNeighbours[4] = m_oldNode;
		m_ptrNeighbours[7] = m_oldNode->m_ptrNeighbours[6];
		m_ptrNeighbours[6] = m_oldNode->m_ptrNeighbours[5];
		m_ptrNeighbours[1] = m_oldNode->m_ptrNeighbours[0];
		//nodes to delete: 2, 3, 7
		delete m_oldNode->m_ptrNeighbours[2];
		delete m_oldNode->m_ptrNeighbours[3];
		delete m_oldNode->m_ptrNeighbours[7];
	}
	else if(m_gridIndex == 4){
		m_ptrNeighbours[0] = m_oldNode->m_ptrNeighbours[1];
		m_ptrNeighbours[1] = m_oldNode->m_ptrNeighbours[2];
		m_ptrNeighbours[3] = m_oldNode;
		m_ptrNeighbours[5] = m_oldNode->m_ptrNeighbours[6];
		m_ptrNeighbours[6] = m_oldNode->m_ptrNeighbours[7];
		//nodes to delete: 0, 3, 5
		delete m_oldNode->m_ptrNeighbours[0];
		delete m_oldNode->m_ptrNeighbours[3];
		delete m_oldNode->m_ptrNeighbours[5];
	}
	else if(m_gridIndex == 6){
		m_ptrNeighbours[0] = m_oldNode->m_ptrNeighbours[3];
		m_ptrNeighbours[1] = m_oldNode;
		m_ptrNeighbours[2] = m_oldNode->m_ptrNeighbours[4];
		m_ptrNeighbours[3] = m_oldNode->m_ptrNeighbours[5];
		m_ptrNeighbours[4] = m_oldNode->m_ptrNeighbours[7];
		//nodes to delete: 0, 1, 2
		delete m_oldNode->m_ptrNeighbours[0];
		delete m_oldNode->m_ptrNeighbours[1];
		delete m_oldNode->m_ptrNeighbours[2];
	}
	*/


	SquareNode* aux;
	int cont = 0;

	if(numNeighbours > 0){
		for(float i=m_position.GetX() - m_size; i<=m_position.GetX() + m_size; i+=m_size){
			for(float j=m_position.GetY() - m_size; j<=m_position.GetY() + m_size; j+=m_size){
				
				if(i != m_position.GetX() || j != m_position.GetY()){

					//Check if the node was already created (and set up on the previous ifs/else ifs
					if(m_ptrNeighbours[cont] == NULL){
						aux = new SquareNode(m_ptrTerrainGenerationShader, m_ptrTerrainRenderingShader, m_ptrFBO, Vector3<float>(i,j,0),m_size, m_numDivisions);
						aux->m_gridIndex = cont;
						m_ptrNeighbours[cont] = aux;
						

						//TODO: make sure that, when the neighbour generate its neighbours, it doesn't create nodes in position already created
						aux->GenerateNeighbours(this, numNeighbours-1);
					}
					else{
						m_ptrNeighbours[cont]->GenerateNeighbours(this, numNeighbours - 1);
					}

					cont++;
				}
			}
		}

	}
	


}
