/*
  ==============================================================================

    ObjectsHolder.cpp
    Created: 11 Apr 2012 5:10:20pm
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
#include "../Models/MDLFile.h"
#include "ContentComp.h"
#include "ObjectComponent.h"
#include "../Controller/ObjController.h"
#include "VariablesPanel.h"

#include "ObjectsHolder.h"

ObjectsHolder::ObjectsHolder(ObjController& objController_)
: objController(objController_),
  mdlFile(nullptr),
  dragging(false)
{
	setSize(100,100);
}

ObjectsHolder::~ObjectsHolder()
{
	if(mdlFile != nullptr)
	{
		mdlFile->removeChangeListener(this);
	}
}


void ObjectsHolder::paint(Graphics& g)
{
	g.fillAll (Colours::white);

	g.setColour(Colours::black);
	if(dragging)
	{
		float x = draggingStart.x;
		float y = draggingStart.y;
		float w = draggingActual.x;// - x;
		float h = draggingActual.y;// - y;
		g.drawRect(x, y, w, h);
	}
}

void ObjectsHolder::resized()
{
	updateComponents();
}

void ObjectsHolder::changeListenerCallback(ChangeBroadcaster*)
{
	updateComponents();
}

void ObjectsHolder::updateComponents()
{
	int i;
    for (i = getNumChildComponents(); --i >= 0;)
    {
        ObjectComponent* const bobj = dynamic_cast <ObjectComponent*> (getChildComponent (i));

        if (bobj != nullptr)
            bobj->update();
    }

}

void ObjectsHolder::mouseDrag (const MouseEvent& e)
{
	draggingActual.x = e.getOffsetFromDragStart().x;
	draggingActual.y = e.getOffsetFromDragStart().y;

	dragging = true;
	repaint();
}

void ObjectsHolder::mouseUp (const MouseEvent& e)
{
	if( e.mouseWasClicked())
	{
		for (int i = getNumChildComponents(); --i >= 0;){
			ObjectComponent* oc = dynamic_cast<ObjectComponent*>(getChildComponent(i));
			oc->setSelected(false);
		}

	}
	else if (! e.mouseWasClicked())
	{
            // object changed
		int x = draggingStart.x;
		int y = draggingStart.y;
		int w = draggingActual.x;// - x;
		int h = draggingActual.y;// - y;

		Rectangle<int> tmpRect(x, y, w, h);
		objController.selectObjectsWithinRectagle(tmpRect);
		dragging = false;
		repaint();
	}
}

void ObjectsHolder::mouseDown (const MouseEvent& e)
{
	draggingStart.x = e.getMouseDownX();
	draggingStart.y = e.getMouseDownY();
//	dragging = true;


}

void ObjectsHolder::setMDLFile(MDLFile* newMDLFile)
{
	if(newMDLFile != mdlFile && newMDLFile != nullptr)
	{
		mdlFile = newMDLFile;
		mdlFile->addChangeListener(this);
		objController.loadComponents(this);
	}
}

void ObjectsHolder::moveObjectsData(Point<int> offset)
{
	objController.moveObjects(this, offset);
}

void ObjectsHolder::moveObjectComponents(Point<int> offset)
{
	for (int i = 0; i < getNumChildComponents(); ++i) {
		ObjectComponent* oc = dynamic_cast<ObjectComponent*>(getChildComponent(i));
		if(oc->selected() && oc != ObjectComponent::isLastClicked)
		{
			oc->mouseDragPassive(offset);
		}
	}
}

bool ObjectsHolder::dispatchMenuItemClick(const ApplicationCommandTarget::InvocationInfo& info)
{
	Point<int> mp = getMouseXYRelative();

	if(mp.x < 0)
		mp.x = 0;
	else if(mp.x > getWidth())
		mp.x = getWidth();
	if(mp.y < 0)
		mp.y = 0;
	else if(mp.y > getHeight())
		mp.y = getHeight();

	switch(info.commandID)
	{
	case StandardApplicationCommandIDs::cut:
		// TODO: implement cut
		break;
	case StandardApplicationCommandIDs::copy:
		// TODO: implement copy
		break;
	case StandardApplicationCommandIDs::paste:
		// TODO: implement paste
		break;
	case StandardApplicationCommandIDs::selectAll:
		objController.selectAll(true);
		break;
	case StandardApplicationCommandIDs::deselectAll:
		objController.selectAll(false);
		break;
	case StandardApplicationCommandIDs::del:
		objController.removeObject(this);
		break;
    case CommandIDs::defineVariables:
    	VariablesPanel::show(mdlFile->mdlRoot, &mdlFile->getUndoMgr());
    	break;
    case CommandIDs::segmentedConnectors:
    	// TODO: implement segmented connectors
    	StoredSettings::getInstance()->setIsSegmentedConnectors(!StoredSettings::getInstance()->getIsSegmentedConnectors());
    	break;
    case CommandIDs::zoomIn:
    	// TODO: implement zoom in
    	break;
    case CommandIDs::zoomOut:
    	// TODO: implement zoom out
    	break;
    case CommandIDs::zoomNormal:
    	// TODO: implement zoom normal
    	break;
    case CommandIDs::reverseDirection:
    	// TODO: implement reverse direction
    	break;

    case CommandIDs::insertMass:
    	objController.addObject(this, Ids::mass, mp.x, mp.y);
    	break;
    case CommandIDs::insertGround:
    	objController.addObject(this, Ids::ground, mp.x, mp.y);
    	break;
    case CommandIDs::insertResonator:
    	objController.addObject(this, Ids::resonator, mp.x, mp.y);
    	break;
    case CommandIDs::insertPort:
    	objController.addObject(this, Ids::port, mp.x, mp.y);
    	break;

    case CommandIDs::insertLink:
    	objController.addObject(this, Ids::link, mp.x, mp.y);
    	break;
    case CommandIDs::insertTouch:
    	objController.addObject(this, Ids::touch, mp.x, mp.y);
    	break;
    case CommandIDs::insertPluck:
    	objController.addObject(this, Ids::pluck, mp.x, mp.y);
    	break;

    case CommandIDs::insertAudioOutput:
    	objController.addObject(this, Ids::audioout, mp.x, mp.y);
    	break;
    case CommandIDs::insertWaveguide:
    	objController.addObject(this, Ids::waveguide, mp.x, mp.y);
    	break;
    case CommandIDs::insertTermination:
    	objController.addObject(this, Ids::termination, mp.x, mp.y);
    	break;
    default:
    	return false;
	}
	return true;
}

void ObjectsHolder::editObjectProperties(ObjectComponent* oc)
{
	objController.editObjectProperties(oc, &mdlFile->getUndoMgr());
}
