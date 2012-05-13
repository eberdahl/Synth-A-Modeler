/*
  ==============================================================================

    ObjController.cpp
    Created: 13 Apr 2012 12:06:25am
    Author:  Peter Vasil

  ==============================================================================

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

*/

#include "../Application/CommonHeaders.h"
#include "../Models/ObjectActions.h"
#include "../Models/MDLFile.h"
#include "MDLController.h"
#include "../View/BaseObjectComponent.h"

#include "ObjController.h"

ObjController::ObjController(MDLController& owner_)
: owner(owner_)
{

}

ObjController::~ObjController()
{
	objects.clear(true);
}

bool ObjController::perform (UndoableAction* const action, const String& actionName)
{
	return owner.perform(action, actionName);
}

void ObjController::addObject(Component* holder, const Identifier& objId,
		int posX, int posY)
{
	const Identifier& tmpIdent = Objects::getObjectType(objId);
	if(tmpIdent != Objects::invalid)
	{
		ValueTree mdl = owner.getMDLTree();
		ValueTree subTree = mdl.getOrCreateChildWithName(tmpIdent, nullptr);
		BaseObjectComponent* objComp = new BaseObjectComponent(objId, posX, posY);
		objects.add(objComp);
		this->perform(new AddObjectAction(holder, objComp,
				subTree, objId, posX, posY), "Add new Object");
	}
}


void ObjController::removeObject(const String& objName)
{
	MDLFile* const mf = owner.getMDLFile();
	ValueTree childToRemove = mf->getObjectWithName(objName);
	if(childToRemove.isValid())
	{
		this->perform(new RemoveObjectAction(childToRemove.getParent(), childToRemove), "Remove Object");
//		subTree.removeChild(childToRemove, owner.getUndoManager());
	}
}
