#include "KeyAnimating.h"

////////////////////////////////////////////////////////////////////////////////

static KeyAnimating *PROJECT=0;
bool fillMode = true;

int main(int argc, char **argv) {
	glutInit(&argc, argv);

	PROJECT=new KeyAnimating("Animation",argc,argv);
	glutMainLoop();
	delete PROJECT;

	return 0;
}

// These are really HACKS to make glut call member functions instead of static functions
static void display()									{PROJECT->Draw();}
static void idle()										{PROJECT->Update();}
static void resize(int x,int y)							{PROJECT->Resize(x,y);}
static void keyboard(unsigned char key,int x,int y)		{PROJECT->Keyboard(key,x,y);}
static void mousebutton(int btn,int state,int x,int y)	{PROJECT->MouseButton(btn,state,x,y);}
static void mousemotion(int x, int y)					{PROJECT->MouseMotion(x,y);}

KeyAnimating::KeyAnimating(const char *windowTitle,int argc,char **argv) {
	WinX=800;
	WinY=600;
	LeftDown=MiddleDown=RightDown=false;
	MouseX=MouseY=0;

	// Create the window
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowSize( WinX, WinY );
	glutInitWindowPosition( 100, 100 );
	WindowHandle = glutCreateWindow( windowTitle );
	glutSetWindowTitle( windowTitle );
	glutSetWindow( WindowHandle );

	// Background color
	glClearColor( 0., 0., 0., 1. );

	// Callbacks
	glutDisplayFunc( display );
	glutIdleFunc( idle );
	glutKeyboardFunc( keyboard );
	glutMouseFunc( mousebutton );
	glutMotionFunc( mousemotion );
	glutPassiveMotionFunc( mousemotion );
	glutReshapeFunc( resize );

	// Initialize GLEW
	glewInit();
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	// Initialize components
	Program = new ShaderProgram("Model.glsl", ShaderProgram::eRender);

	// setup skeleton
	Skel=new Skeleton;
	jointNo = 0;				// for interactivity
	char* skelName = argv[1];
	char* skinName = argv[2];
	char* animName = argv[3];
	/*if (argc > 1) {
		fprintf(stdout, "\nLoading %s.\n", skelName);
		Skel->Load(skelName);
	}
	else {
		fprintf(stdout, "\n No skeleton file chosen. Loading default wasp_walk.skel.\n");
		Skel->Load("wasp_walk.skel");
	}*/
	Skel->Load("wasp_walk.skel");
	numJoints = Skel->jointTree.size();
	jointMode = 0;


	// setup skin
	skin = new Skin(Skel);
	/*if (argc > 2) {
		fprintf(stdout, "\nLoading %s.\n", skinName);
		skin->Load(skinName);
	}
	else {
		fprintf(stdout, "\nNo Skin File chosen. Loading default wasp_walk.skin.\n");
		skin->Load("wasp_walk.skin");
	}*/
	skin->Load("wasp_walk.skin");
	// setup animation clip
	Clip = new Animation();
	/*if (argc > 3) {
		fprintf(stdout, "\nLoading %s.\n", animName);
		Clip->Load(animName);
	}
	else {
		fprintf(stdout, "\nNo animation file chosen. Loading default wasp_walk.anim\n");
	}*/
	Clip->Load("wasp_walk.anim");

	// setup animation player
	player = new Player();
	player->setSkel(Skel);
	player->setClip(Clip);

	Cam=new Camera;
	Cam->SetAspect(float(WinX)/float(WinY));
	printf("done loading\n\n");
}

KeyAnimating::~KeyAnimating() {
	delete Program;
	delete Skel;
	delete skin;
	delete Cam;

	glFinish();
	glutDestroyWindow(WindowHandle);
}

void KeyAnimating::Update() {
	// Update the components in the world
	Skel->Update();
	skin->Update();
	player->Update();
	
	Cam->Update();

	// Tell glut to re-display the scene
	glutSetWindow(WindowHandle);
	glutPostRedisplay();
}

void KeyAnimating::Reset() {
	player->Reset();
	Cam->Reset();
	Cam->SetAspect(float(WinX)/float(WinY));
}

void KeyAnimating::Draw() {
	// Begin drawing scene
	glViewport(0, 0, WinX, WinY);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw components
	skin->Draw(Cam->GetViewProjectMtx(), Program->GetProgramID());

	// Finish drawing scene
	glFinish();
	glutSwapBuffers();
}

void KeyAnimating::Quit() {
	glFinish();
	glutDestroyWindow(WindowHandle);
	exit(0);
}

void KeyAnimating::Resize(int x,int y) {
	WinX = x;
	WinY = y;
	Cam->SetAspect(float(WinX)/float(WinY));
}

void KeyAnimating::Keyboard(int key,int x,int y) {
	switch(key) {
		case 0x1b:		// Escape
			Quit();
			break;
		case 't':		// toggle wireframe on/off
			fillMode = !fillMode;
			if (fillMode) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				printf("toggle fill\n");
			}
			else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				printf("toggle wireframe\n");
			}
			break;
	}
}

void KeyAnimating::MouseButton(int btn,int state,int x,int y) {
	if(btn==GLUT_LEFT_BUTTON) {
		LeftDown = (state==GLUT_DOWN);
	}
	else if(btn==GLUT_MIDDLE_BUTTON) {
		MiddleDown = (state==GLUT_DOWN);
	}
	else if(btn==GLUT_RIGHT_BUTTON) {
		RightDown = (state==GLUT_DOWN);
	}
}

void KeyAnimating::MouseMotion(int nx,int ny) {
	int maxDelta=100;
	int dx = glm::clamp(nx - MouseX,-maxDelta,maxDelta);
	int dy = glm::clamp(-(ny - MouseY),-maxDelta,maxDelta);

	MouseX = nx;
	MouseY = ny;

	// Move camera
	// NOTE: this should really be part of Camera::Update()
	if(LeftDown) {
		const float rate=1.0f;
		Cam->SetAzimuth(Cam->GetAzimuth()+dx*rate);
		Cam->SetIncline(glm::clamp(Cam->GetIncline()-dy*rate,-90.0f,90.0f));
	}
	if(RightDown) {
		const float rate=0.005f;
		float dist=glm::clamp(Cam->GetDistance()*(1.0f-dx*rate),0.01f,1000.0f);
		Cam->SetDistance(dist);
	}
}