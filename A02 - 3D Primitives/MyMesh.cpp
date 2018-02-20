#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	//create top of cone
	vector3 topPoint(0.0f, a_fHeight/2, 0.0f);

	//create bottom point of cone
	vector3 bottomPoint(0.0f, -a_fHeight / 2, 0.0f);

	//get starter angle
	float angle = 360.0f / a_nSubdivisions;
	float originalAngle = angle;
	angle = 0.0f;

	//create base points
	std::vector<vector3> basePoints;
	
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		basePoints.push_back(vector3(sin(angle* PI/180)*a_fRadius, -a_fHeight/2, -cos(angle* PI / 180)* a_fRadius));
		angle += originalAngle;
	}

	//connect points
	for (uint i = 0; i < basePoints.size(); i++)
	{
		if (i == basePoints.size() - 1)
		{
			AddTri(basePoints[i], basePoints[0], bottomPoint);
			AddTri(basePoints[0], basePoints[i], topPoint);
		}
		else
		{
			AddTri(basePoints[i], basePoints[i+1], bottomPoint);
			AddTri(basePoints[i+1], basePoints[i], topPoint);
		}
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	//create top of cylinder
	vector3 topPoint(0.0f, a_fHeight / 2, 0.0f);

	//create bottom point of ccylinder
	vector3 bottomPoint(0.0f, -a_fHeight / 2, 0.0f);

	//get starter angle
	float angle = 360.0f / a_nSubdivisions;
	float originalAngle = angle;
	angle = 0.0f;


	//create base points
	std::vector<vector3> basePoints;

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		basePoints.push_back(vector3(sin(angle* PI / 180)*a_fRadius, -a_fHeight / 2, -cos(angle* PI / 180)* a_fRadius));
		angle += originalAngle;
	}
	//reset angle
	angle = 0.0f;

	//create top points
	std::vector<vector3> topPoints;

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		topPoints.push_back(vector3(sin(angle* PI / 180)*a_fRadius, a_fHeight / 2, -cos(angle* PI / 180)* a_fRadius));
		angle += originalAngle;
	}

	//connect base points
	for (uint i = 0; i < basePoints.size(); i++)
	{
		if (i == basePoints.size() - 1)
		{
			AddTri(basePoints[i], basePoints[0], bottomPoint);
		}
		else
		{
			AddTri(basePoints[i], basePoints[i + 1], bottomPoint);
		}
	}

	//connect top points
	for (uint i = 0; i < topPoints.size(); i++)
	{
		if (i == topPoints.size() - 1)
		{
			AddTri(topPoints[0], topPoints[i], topPoint);
		}
		else
		{
			AddTri(topPoints[i+1], topPoints[i], topPoint);
		}
	}

	//connect base to top
	for (uint i = 0; i < topPoints.size(); i++)
	{
		if (i == topPoints.size() - 1)
		{
			AddQuad(topPoints[i], topPoints[0], basePoints[i], basePoints[0]);
		}
		else
		{
			AddQuad(topPoints[i], topPoints[i + 1], basePoints[i], basePoints[i + 1]);
		}
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	//GenerateCube(a_fOuterRadius * 2.0f, a_v3Color);

	//get starter angle
	float angle = 360.0f / a_nSubdivisions;
	float originalAngle = angle;
	angle = 0.0f;


	//create points at top inside of tube
	std::vector<vector3> topInnerPoints;
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		topInnerPoints.push_back(vector3(sin(angle * PI / 180) * a_fInnerRadius, a_fHeight, cos(angle * PI / 180) * a_fInnerRadius));
		angle += originalAngle;
	}

	//reset angle
	angle = 0.0f;

	//create points at bootom inside of tube
	std::vector<vector3> bottomInnerPoints;
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		bottomInnerPoints.push_back(vector3(sin(angle * PI / 180) * a_fInnerRadius, -a_fHeight, cos(angle * PI / 180) * a_fInnerRadius));
		angle += originalAngle;
	}

	//reset angle
	angle = 0.0f;

	//cretae points at top outside of tube
	std::vector<vector3> topOutsidePoints;
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		topOutsidePoints.push_back(vector3(sin(angle * PI / 180) * a_fOuterRadius, a_fHeight, cos(angle * PI / 180) * a_fOuterRadius));
		angle += originalAngle;
	}

	//reset angle
	angle = 0.0f;

	//cretae points at bottom outside of tube
	std::vector<vector3> bottomOutsidePoints;
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		bottomOutsidePoints.push_back(vector3(sin(angle * PI / 180) * a_fOuterRadius, -a_fHeight, cos(angle * PI / 180) * a_fOuterRadius));
		angle += originalAngle;
	}


	//connect inside points
	for (uint i = 0; i < topInnerPoints.size(); i++)
	{
		if (i == topInnerPoints.size() - 1)
		{
			AddQuad(topInnerPoints[i], topInnerPoints[0], bottomInnerPoints[i], bottomInnerPoints[0]);
		}
		else
		{
			AddQuad(topInnerPoints[i], topInnerPoints[i + 1], bottomInnerPoints[i], bottomInnerPoints[i + 1]);
		}
	}

	//connect outside points
	for (uint i = 0; i < topOutsidePoints.size(); i++)
	{
		if (i == topOutsidePoints.size() - 1)
		{
			AddQuad(bottomOutsidePoints[i], bottomOutsidePoints[0], topOutsidePoints[i], topOutsidePoints[0]);
		}
		else
		{
			AddQuad(bottomOutsidePoints[i], bottomOutsidePoints[i + 1], topOutsidePoints[i], topOutsidePoints[i + 1]);
		}
	}

	//connect bottom points
	for (uint i = 0; i < bottomInnerPoints.size(); i++)
	{
		if (i == bottomInnerPoints.size() - 1)
		{
			AddQuad(bottomInnerPoints[i], bottomInnerPoints[0], bottomOutsidePoints[i], bottomOutsidePoints[0]);
		}
		else
		{
			AddQuad(bottomInnerPoints[i], bottomInnerPoints[i + 1], bottomOutsidePoints[i], bottomOutsidePoints[i + 1]);
		}
	}

	//connect top points
	for (uint i = 0; i < topInnerPoints.size(); i++)
	{
		if (i == topInnerPoints.size() - 1)
		{
			AddQuad(topOutsidePoints[i], topOutsidePoints[0], topInnerPoints[i], topInnerPoints[0]);
		}
		else
		{
			AddQuad(topOutsidePoints[i], topOutsidePoints[i + 1], topInnerPoints[i], topInnerPoints[i + 1]);
		}
	}


	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	//get radius of the tubing
	float radiusTube = (a_fOuterRadius - a_fInnerRadius)/2;
	float radiusToCenter = a_fInnerRadius + radiusTube;

	//get angle for the vertical subdivisions
	float angleVert = 360.0f / a_nSubdivisionsA;
	float originalAngleVert = angleVert;
	angleVert = 0.0f;

	//get angle for the horizontal subs
	float angleHoriz = 360.0f / a_nSubdivisionsB;
	float originalAngleHoriz = angleHoriz;
	angleHoriz = 0.0f;

	//setup container of points
	std::vector<std::vector<vector3>> points(a_nSubdivisionsA);

	//add points
	for (int i = 0; i < a_nSubdivisionsA; i++)
	{
		for (int j = 0; j < a_nSubdivisionsB; j++)
		{
			points[i].push_back(vector3(
				cos(angleVert*PI/180)*radiusToCenter + cos(angleHoriz*PI/180)*radiusTube*cos(angleVert*PI/180), 
				sin(angleHoriz*PI/180)*radiusTube, 
				-sin(angleVert*PI / 180)*radiusToCenter +  cos(angleHoriz*PI / 180)*radiusTube*-sin(angleVert*PI / 180)
			));
			angleHoriz += originalAngleHoriz;
		}

		angleHoriz = 0.0f;
		angleVert += originalAngleVert;
	}

	//connect points
	for (uint i = 0; i < points.size(); i++)
	{
		if (i == points.size() - 1)
		{
			for (uint j = 0; j < points[i].size(); j++)
			{
				if (j == points[i].size() - 1)
				{
					AddQuad(points[i][j], points[0][j], points[i][0], points[0][0]);
				}
				else
				{
					AddQuad(points[i][j], points[0][j], points[i][j + 1], points[0][j + 1]);
				}
			}
		}
		else
		{
			for (uint j = 0; j < points[i].size(); j++)
			{
				if (j == points[i].size() - 1)
				{
					AddQuad(points[i][j], points[i + 1][j], points[i][0], points[i + 1][0]);
				}
				else
				{
					AddQuad(points[i][j], points[i + 1][j], points[i][j + 1], points[i + 1][j + 1]);
				}
			}
		}
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;

	Release();
	Init();

	//setup top and bottom points
	vector3 topPoint(0.0f, a_fRadius, 0.0f);
	vector3 bottomPoint(0.0f, -a_fRadius, 0.0f);

	//make vector containing vector of points
	std::vector<std::vector<vector3>> points(a_nSubdivisions);

	//setup angle to use for x and z
	float angle = 360.0f / a_nSubdivisions;
	float originalAngle = angle;
	angle = 0.0f;

	//setup angle to determine point distance from center
	float angleRadius = 180.0f/ a_nSubdivisions;
	float originalAngleRadius = angleRadius;


	//setup points
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		for (int j = 0; j < a_nSubdivisions; j++)
		{			
			points[i].push_back(vector3(sin(angle* PI / 180)* sin(angleRadius*PI/180)*a_fRadius, cos(angleRadius*PI/180)* -a_fRadius, cos(angle*PI / 180) *sin(angleRadius*PI / 180)* a_fRadius));
			angle += originalAngle;
		}

		//reset angle
		angle = 0.0f;

		//increment angleRadius
		angleRadius += originalAngleRadius;
	}

	//connect bottom points
	for (uint i = 0; i < points[0].size(); i++)
	{
		if (i == points[0].size() - 1)
		{
			AddTri(points[0][i], bottomPoint, points[0][0]);
		}
		else
		{
			AddTri(points[0][i], bottomPoint, points[0][i + 1]);
		}
	}

	//connect top points
	for (uint i = 0; i < points[points.size() - 1].size(); i++)
	{
		if (i == points[points.size() - 1].size() - 1)
		{
			AddTri(points[points.size() - 1][i], points[points.size() - 1][0], topPoint);
		}
		else
		{
			AddTri(points[points.size() - 1][i], points[points.size() - 1][i + 1], topPoint);
		}
	}

	//connect points
	for (uint i = 0; i < points.size() - 1; i++)
	{
		for (uint j = 0; j < points[i].size(); j++)
		{
			if (j == points[i].size() - 1)
			{
				AddQuad(points[i][j], points[i][0], points[i + 1][j], points[i + 1][0]);
			}
			else
			{
				AddQuad(points[i][j], points[i][j + 1], points[i + 1][j], points[i + 1][j + 1]);
			}

		}
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}