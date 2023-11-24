/*
   File:        PageHallOfFame.cpp
  Description: Hall of Fame page
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

#include "Menu.h"



void PageHallOfFame::Prepare(int iParam,void *pParam) {

  nbItem  = 10;
  if(iParam<10)
    selItem = iParam;
  else
    selItem = 0;

  strcpy(editText,"          ");
  editPos = 0;
  startEditTime = 0.0f;
  editCursor = FALSE;
  editScore = (SCOREREC *)pParam;
  mParent->GetSetup()->GetHighScore(allScore);

  if( editScore ) {
    editMode = TRUE;
  } else {
    editMode = FALSE;
  }

}

PLAYER_INFO *PageHallOfFame::GetPlayer() {

  strcpy(pInfo.name , allScore[selItem].name);
  int i = (int)strlen(pInfo.name) - 1;
  while(i>=0 && pInfo.name[i]==' ') {
    pInfo.name[i]=0;
    i--;
  }
  pInfo.highScore = allScore[0].score;
  pInfo.rank = selItem + 1;

  return &pInfo;

}

void PageHallOfFame::Render() {

  char tmp[256];

  sprintf(tmp,"HALL OF FAME %s",mParent->GetSetup()->GetName());
  mParent->RenderTitle(tmp);
  for(int i=0;i<10;i++) {
    sprintf(tmp,"%2d ",i+1);
    mParent->RenderText(0,i,(selItem==i) && !editMode,tmp);
    mParent->RenderText(3,i,(selItem==i) && !editMode,allScore[i].name);
    sprintf(tmp,"%7d ",allScore[i].score);
    mParent->RenderText(13,i,(selItem==i) && !editMode,tmp);
    sprintf(tmp,"[%s]",FormatDateShort(allScore[i].date));
    mParent->RenderText(21,i,(selItem==i) && !editMode,tmp);
  }
  // Edition mode
  if( editMode ) {
    mParent->RenderText(3,selItem,FALSE,editText);
    mParent->RenderText(3+editPos,selItem,editCursor,STR(" "));
  }

}

int PageHallOfFame::Process(BYTE *keys,float fTime) {

  if( !editMode ) {

    ProcessDefault(keys,fTime);

    if( keys[BO_RETURN] ) {
      mParent->ToPage(&mParent->scoreDetailsPage,selItem,allScore + selItem);
      keys[BO_RETURN] = 0;
    }

    if( keys[BO_ESCAPE] ) {
       mParent->ToPage(&mParent->mainMenuPage);
       keys[BO_ESCAPE] = 0;
    }

  } else {
    ProcessEdit(keys,fTime);
  }

  return 0;

}

// ---------------------------------------------------------------------

void PageHallOfFame::ProcessEdit(BYTE *keys,float fTime) {
  
  if( startEditTime == 0.0f )
    startEditTime = fTime;

  editCursor = ( (fround((startEditTime - fTime) * 2.0f)%2) == 0 );

  char c = GetChar(keys);
  if( c>0 && editPos<10 ) {
    editText[editPos] = c;
    editPos++;
  }
  
  // Delete
  if( keys[BO_DELETE] || keys[BO_LEFT] || keys[BO_BACKSPACE] ) {
    if( editPos>0 ) editPos--;
    if( editPos<10 ) editText[editPos]=' ';
    keys[BO_DELETE] = 0;
    keys[BO_LEFT] = 0;
    keys[BO_BACKSPACE] = 0;
  }

  if( keys[BO_ESCAPE] || keys[BO_RETURN] ) {
    // Record new name and save
    strcpy(editScore->name,editText);
    mParent->GetSetup()->SaveHighScore();
    mParent->GetSetup()->GetHighScore(allScore);
    editMode = FALSE;
    keys[BO_ESCAPE] = 0;
    keys[BO_RETURN] = 0;
  }

}

