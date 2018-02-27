#include "AppClass.h"
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Nicholas Henderson - nch8501@rit.edu";
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUp(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}
	
	//could not get reading from .ini file to work
	m_uOrbits = GetPrivateProfileInt("Variables", "orbits", 10, ".\LERP.ini");


	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;


	float fSize = 1.0f; //initial size of orbits

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	uint uSides = 3; //start with the minimal 3 sides
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager
		fSize += 0.5f; //increment the size for the next orbit
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
	}
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system
	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	m4Offset = glm::rotate(IDENTITY_M4, 90.0f, AXIS_Z);

	/*
	|	MY CODE
	|	MY CODE
	|	MY CODE
	|	MY CODE
	|	MY CODE
	|	MY CODE
	*/

	//create a percent to figure out where to position the sphere between the vertices
	static float fPercent = 0.0f;

	//create a container for the vertices
	static std::vector <std::vector<vector3>> vertices(m_shapeList.size());
	if (vertices[0].empty())
	{
		//get all the vertices of the orbits
		for (uint i = 3; i < m_shapeList.size() + 3; i++)
		{
			//get the angle
			float fAngle = 360.0f / i;
			float originalAngle = 0.0f;

			//get the size of the orbit
			float orbitSize = 1.0f + (i - 3)*.5f;

			//get the points
			for (uint j = 0; j < i; j++)
			{
				//find the vertex
				vector3 point = vector3(cos(originalAngle*PI / 180)* orbitSize, sin(originalAngle*PI / 180)* orbitSize, 0.0f);

				//add the vertex to the vector
				vertices[i - 3].push_back(point);
				originalAngle += fAngle;
			}
		}
	}

	//create a tracker to find the next vertex to go to
	static std::vector<int> nextVertex(vertices.size());
	if (vertices.empty())
	{
		for (uint i = 0; i < nextVertex.size(); i++)
		{
			nextVertex[i] = 1;
		}
	}

	
	//draw the spheres and orbits
	for (uint i = 0; i < vertices.size(); i++)
	{
		//set the orbit to draw
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 90.0f, AXIS_X));

		//current location of sphere
		vector3 currentLoc;

		//get the sphere's current location
		if (nextVertex[i] == 0)
		{
			currentLoc = glm::lerp(vertices[i][vertices[i].size()-1], vertices[i][nextVertex[i]], fPercent);
		}
		else if (nextVertex[i] >= vertices[i].size())
		{
			
			currentLoc = glm::lerp(vertices[i][nextVertex[i] - 1], vertices[i][0], fPercent);
			nextVertex[i] = 0;
		}
		else
		{
			currentLoc = glm::lerp(vertices[i][nextVertex[i] - 1], vertices[i][nextVertex[i]], fPercent);
		}

		//set the sphere to draw
		matrix4 m4Model = glm::translate(m4Offset, currentLoc);
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);

	}

	//increment fPercent
	if (fPercent >= 1.0f)
	{
		fPercent = 0.0f;

		//increment nextVertex
		for (int i = 0; i < nextVertex.size(); i++)
		{
			//check if the nextVertex doesn't exist
			if (nextVertex[i] >= vertices[i].size())
			{
				nextVertex[i] = 0;
			}
			else
			{
				nextVertex[i] += 1;
			}
		}
	}
	else
	{
		fPercent += 0.025f;
	}


	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release GUI
	ShutdownGUI();
}