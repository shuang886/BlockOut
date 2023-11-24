/*
  File:        BlockOut.cpp
  Description: Main application class
  Program:     BlockOut
  Author:      Jean-Luc PONS

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
*/

#include "BlockOut.h"

#ifndef WINDOWS
#include <unistd.h>
#include <OpenGL/glu.h>
#else
extern void InitialiseWinsock();
#endif

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
#ifdef WINDOWS
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
#else
int main(int argc,char *argv[])
#endif
{

  // Check environement
  if( !CheckEnv() ) {
    return 0;
  }

#ifdef WINDOWS
  InitialiseWinsock();
#endif
    
  // Create and start the application
  BlockOut *glApp = new BlockOut();
    
  if (!glApp->Create(glApp->theSetup.GetWindowWidth(),
	  glApp->theSetup.GetWindowHeight(),
	  glApp->theSetup.GetFullScreen(),
	  glApp->theSetup.GetFrLimiter() == FR_LIMITVSYNC)) {
	  delete glApp;
	  return 0;
  }

  return glApp->Run();
}

//-----------------------------------------------------------------------------
// Name: BlockOut()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
BlockOut::BlockOut()
{
  inited = FALSE;
  mode = MENU_MODE;
  m_strWindowTitle = STR(APP_VERSION);
  ZeroMemory( m_bKey, sizeof(m_bKey) );
}

//-----------------------------------------------------------------------------
// Name: UpdateFullScreen()
// Desc: Update fullscreen
//-----------------------------------------------------------------------------
int BlockOut::UpdateFullScreen()
{
  int hr = GL_OK;

  if( m_bWindowed ) {
    if( theSetup.GetFullScreen() ) {
      // Go to fullscreen
      Pause(TRUE);
      hr = ToggleFullscreen();
      Pause(FALSE);
    }
  } else {
    if( !theSetup.GetFullScreen() ) {
      // Go to windowed
      Pause(TRUE);
      hr = ToggleFullscreen();
      Pause(FALSE);
    }
  }

  return hr;

}

//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
int BlockOut::FrameMove()
{

  // General keys

  if( m_bKey[BO_F1] ) {
    BOOL fs = theSetup.GetFullScreen();
    theSetup.SetFullScreen(!fs);
    UpdateFullScreen();
    m_bKey[BO_F1]=0;
  }

  int availWidth, availHeight;
  SDL_GetWindowSize(m_screen, &availWidth, &availHeight);
  
  int width, height;
  double xRatio = (double)availWidth / (double)m_screenWidth;
  double yRatio = (double)availHeight / (double)m_screenHeight;
  if ( xRatio < yRatio )
  {
    width = availWidth;
    height = m_screenHeight * xRatio;
  } else {
    width = m_screenWidth * yRatio;
    height = availHeight;
  }
  
  int xOffset = (availWidth - width) / 2;
  int yOffset = (availHeight - height) / 2;

  // Processing
  int retValue;
  switch(mode) {
    case MENU_MODE:
      retValue = theMenu.Process(m_bKey,m_fTime);
      switch( retValue ) {
        case 1: // Switch to game mode
          ZeroMemory( m_bKey, sizeof(m_bKey) );
          theGame.StartGame(width,height,xOffset,yOffset,m_fTime);
          mode = GAME_MODE;
          break;
        case 2: // Resize
          Resize(theSetup.GetWindowWidth() , theSetup.GetWindowHeight());
          break;
        case 3:
          UpdateFullScreen();
          break;
        case 7:
          theGame.StartDemo(width,height,xOffset,yOffset,m_fTime);
          mode = GAME_MODE;
          break;
        case 8:
          theGame.StartPractice(width,height,xOffset,yOffset,m_fTime);
          mode = GAME_MODE;
          break;
        case 100: // Exit
          InvalidateDeviceObjects();
          BOOL fs = theSetup.GetFullScreen();
          if (fs) {
            theSetup.SetFullScreen(!fs);
            UpdateFullScreen();
          }  
          _exit(0);
          break;
      }
      break;
    case GAME_MODE:

      // Frame limiter
      int toSleep = (int)(theSetup.GetFrLimitTime()*1000.0f);
      if( toSleep>0 ) {
        int elapsed = SDL_GetTicks() - lastSleepTime;
        toSleep -= elapsed;
#ifdef WINDOWS
        if(toSleep>0) Sleep(toSleep);
#else
        if(toSleep>0) usleep(toSleep*1000);
#endif
        lastSleepTime = SDL_GetTicks();
      }

      retValue = theGame.Process(m_bKey,m_fTime);

      switch( retValue ) {
        case 1: {
          // Check High Score
          SCOREREC *score = theGame.GetScore();
          SCOREREC *newScore;
          int pos = theSetup.InsertHighScore(score,&newScore);
          if( newScore ) {
            switch(theSetup.GetSoundType()) {
              case SOUND_BLOCKOUT2:
                theSound.PlayWellDone();
                break;
              case SOUND_BLOCKOUT:
                theSound.PlayWellDone2();
                break;
            }
          }
          theMenu.ToPage(&theMenu.hallOfFamePage,pos,(void *)newScore);
          // Switch to menu mode
          mode = MENU_MODE;
        }
        break;
        case 2: // Return from Demo
          ZeroMemory( m_bKey, sizeof(m_bKey) );
          mode = MENU_MODE;
          theMenu.FullRepaint();
          break;
      }
      break;
  }

  return GL_OK;
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
int BlockOut::Render()
{
    if(!inited) return GL_OK;

    // Begin the scene
    switch( mode ) {
      case MENU_MODE:
        theMenu.Render();
#ifdef _DEBUG
        // Output statistics
        m_pSmallFont.DrawText( 10,  10, m_strFrameStats );
#endif
        break;
      case GAME_MODE:
        theGame.Render();
#ifdef _DEBUG
        // Output statistics
        m_pSmallFont.DrawText( 200,  20, m_strFrameStats );
#endif
        break;
    }

    return GL_OK;
}

//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
int BlockOut::OneTimeSceneInit()
{

  if( !theSound.Create() ) {
#ifdef WINDOWS
    char message[256];
	sprintf(message,"Failed to initialize sound manager.\nNo sound will be played.\n%s\n",theSound.GetErrorMsg());
	MessageBox(NULL,message,"Warning",MB_OK|MB_ICONWARNING);
#else
    printf("Failed to initialize sound manager.\nNo sound will be played.\n%s\n",theSound.GetErrorMsg());
#endif
  }

  // Init default
  theSound.SetEnable(theSetup.GetSound());
  theHttp.SetProxy(theSetup.GetUseProxy());
  theHttp.SetProxyName(theSetup.GetProxyName());
  theHttp.SetProxyPort(theSetup.GetProxyPort());

  // Set manager
  theMenu.SetSetupManager(&theSetup);
  theMenu.SetSoundManager(&theSound);
  theMenu.SetHttp(&theHttp);
  theGame.SetSetupManager(&theSetup);
  theGame.SetSoundManager(&theSound);
  theGame.pFont = &m_pSmallFont;

  return GL_OK;
}

//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
int BlockOut::RestoreDeviceObjects()
{
    GLfloat matView[16];
    GLenum glError;

    int availWidth, availHeight;
    SDL_GetWindowSize(m_screen, &availWidth, &availHeight);
    
    int width, height;
    double xRatio = (double)availWidth / (double)m_screenWidth;
    double yRatio = (double)availHeight / (double)m_screenHeight;
    if ( xRatio < yRatio )
    {
      width = availWidth;
      height = m_screenHeight * xRatio;
    } else {
      width = m_screenWidth * yRatio;
      height = availHeight;
    }
  
    int xOffset = (availWidth - width) / 2;
    int yOffset = (availHeight - height) / 2;

    m_pSmallFont.RestoreDeviceObjects(width,height);

    //Set clear color
    glClearColor( 0, 0, 0, 0 );
 
    // Set viewport
    glViewport(0,0,width,height);

    //Compute view matrix (Right Handed)
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    gluLookAt(0.0,0.0,0.0, 0.0,0.0,10.0, 0.0,1.0,0.0);
    glGetFloatv( GL_MODELVIEW_MATRIX , matView );

    // Light
    glShadeModel(GL_SMOOTH);

    GLfloat global_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

    GLfloat ambientLight[]  = { 0.0f, 0.0f, 0.0f, 1.0f   };
    GLfloat diffuseLight[]  = { 1.0f, 1.0f, 1.0f, 1.0f   };
    GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f   };
    GLfloat position[]      = { 15.0f, 10.0f, -10.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glEnable(GL_LIGHT0);

    // Default for texure
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

    // Default for zbuff
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    //If there was any errors
    glError = glGetError();
    if( glError != GL_NO_ERROR )
    {
        fprintf(stderr, "OpenGL error: %#x\n", glError);
        return GL_FAIL;    
    }
    
    // Set up device objects
    if( !theMenu.Create(width,height,xOffset,yOffset) )
      return GL_FAIL;

    if( !theGame.Create(width,height,xOffset,yOffset) )
      return GL_FAIL;
  
    theGame.SetViewMatrix(matView);
    theMenu.SetViewMatrix(matView);

    inited = TRUE;
    return GL_OK;
}

//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc:
//-----------------------------------------------------------------------------
int BlockOut::InvalidateDeviceObjects()
{
    inited = FALSE;
    m_pSmallFont.InvalidateDeviceObjects();
    theGame.InvalidateDeviceObjects();
    theMenu.InvalidateDeviceObjects();

    return GL_OK;
}

//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Message proc function to handle key and menu input
//-----------------------------------------------------------------------------
int BlockOut::EventProc(SDL_Event *event)
{

  /*
    case WM_PAINT:
    // Need a full repaint
    theGame.FullRepaint();
    theMenu.FullRepaint();
    break;
  */

  // Handle key presses
  if( event->type == SDL_KEYDOWN )
  {
    SDL_Keycode sym = event->key.keysym.sym;
    switch (sym) {
      case SDLK_F1:       m_bKey[BO_F1] = 1; break;
      case SDLK_UP:       m_bKey[BO_UP] = 1; break;
      case SDLK_DOWN:     m_bKey[BO_DOWN] = 1; break;
      case SDLK_LEFT:     m_bKey[BO_LEFT] = 1; break;
      case SDLK_RIGHT:    m_bKey[BO_RIGHT] = 1; break;
      case SDLK_HOME:     m_bKey[BO_HOME] = 1; break;
      case SDLK_END:      m_bKey[BO_END] = 1; break;
      case SDLK_PAGEUP:   m_bKey[BO_PAGEUP] = 1; break;
      case SDLK_PAGEDOWN: m_bKey[BO_PAGEDOWN] = 1; break;
      case SDLK_DELETE:   m_bKey[BO_DELETE] = 1; break;
      default:
        if( sym < 0x80 )
        {
          m_bKey[sym] = 1;
        }
    }
  }

  return GL_OK;

}
