/*
  ==============================================================================

    LinkComponent.cpp
    Created: 21 Aug 2012 11:23:02am
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
#include "../Controller/ObjController.h"
#include "../Graph/Node.h"
#include "BaseObjectComponent.h"
#include "SelectableObject.h"
#include "ObjectComponent.h"
#include "ObjectsHolder.h"

#include "LinkComponent.h"

using namespace synthamodeler;

LinkComponent::LinkComponent(ObjController& owner_, ValueTree linkTree)
: BaseObjectComponent(owner_, linkTree),
    lastInputX (0),
    lastInputY (0),
    lastOutputX (0),
    lastOutputY (0),
    segmented(false)
{
    startComp = owner.getObjectForId(data.getProperty(Ids::startVertex).toString());
    endComp = owner.getObjectForId(data.getProperty(Ids::endVertex).toString());
    
    startComp->addLinkToObject(this);
    endComp->addLinkToObject(this);

    owner.getSelectedObjects().addChangeListener(this);

    segmented = StoredSettings::getInstance()->getIsSegmentedConnectors();

    iconWidth = 25;
    iconHeight = 25;

    color = Colours::green;
    colorSelected = Colours::red;

    if(data.getType() == Ids::link)
    {
        color = Colour(0xff00b700);
    }
    else if(data.getType() == Ids::touch)
    {
        color = Colour(0xff006f00);
    }
    else if(data.getType() == Ids::pulsetouch)
    {
        color = Colour(0xff006f00);
    }
    else if(data.getType() == Ids::pluck)
    {
        color = Colour(0xff006f00);
    }
    else if(data.getType() == Ids::waveguide)
    {
        color = Colour(0xff000090);
        colorSelected = Colours::violet;
    }
    update();
}

LinkComponent::~LinkComponent()
{
    startComp->removeLinkFromObject(this);
    endComp->removeLinkFromObject(this);
    
    owner.getSelectedObjects().removeChangeListener(this);
}
void LinkComponent::update()
{
    float x1, y1, x2, y2;
    getPoints(x1, y1, x2, y2);

    if (lastInputX != x1
        || lastInputY != y1
        || lastOutputX != x2
        || lastOutputY != y2)
    {
        resizeToFit();
    }
}

void LinkComponent::resizeToFit()
{
    float x1, y1, x2, y2;
    getPoints(x1, y1, x2, y2);

    const Rectangle<int> newBounds((int) jmin(x1, x2) - 25,
                                   (int) jmin(y1, y2) - 25,
                                   (int) fabsf(x1 - x2) + 50,
                                   (int) fabsf(y1 - y2) + 50);

    if (newBounds != getBounds())
        setBounds(newBounds);
    else
        resized();

    repaint();
}
void LinkComponent::resized()
{
    float x1 = curInputX;
    float y1 = curInputY;
    float x2 = curOutputX;
    float y2 = curOutputY;
    getPoints(x1, y1, x2, y2);

    lastInputX = x1;
    lastInputY = y1;
    lastOutputX = x2;
    lastOutputY = y2;

    x1 -= getX();
    y1 -= getY();
    x2 -= getX();
    y2 -= getY();

    curInputX = x1;
    curInputY = y1;
    curOutputX = x2;
    curOutputY = y2;
}

void LinkComponent::paint(Graphics& g)
{
    if(selected)
        currentColor = colorSelected;
    else
        currentColor = color;
    drawPath(g);
}

void LinkComponent::mouseDown(const MouseEvent& e)
{
    mouseDownSelectStatus = owner.getSelectedObjects().addToSelectionOnMouseDown (this, e.mods);
}

void LinkComponent::mouseDrag(const MouseEvent& e)
{
    
}

void LinkComponent::mouseUp(const MouseEvent& e)
{
    if (e.mouseWasClicked() && e.getNumberOfClicks() == 2)
	{
		getObjectsHolder()->editObjectProperties(this);
	}
    else if (e.mods.isPopupMenu())
    {
        showContextMenu();
        return; // this may be deleted now..
    }
    owner.getSelectedObjects().addToSelectionOnMouseUp (this, e.mods, false, 
                                                      mouseDownSelectStatus);
    update();
}

bool LinkComponent::hitTest(int x, int y)
{
    if (hitPath.contains((float) x, (float) y))
    {
        double distanceFromStart, distanceFromEnd;
        getDistancesFromEnds(x, y, distanceFromStart, distanceFromEnd);

        // avoid clicking the connector when over a pin
        return distanceFromStart > 7.0 && distanceFromEnd > 7.0;
    }

    return false;
}

void LinkComponent::setSelected(bool shouldBeSelected)
{
    selected = shouldBeSelected;
    if(selected)
    {
        owner.getSelectedObjects().addToSelection(this);
    }
    else
    {
        owner.getSelectedObjects().deselect(this);
    }
}

void LinkComponent::getPoints(float& x1, float& y1, float& x2, float& y2) const
{
    x1 = lastInputX;
    y1 = lastInputY;
    x2 = lastOutputX;
    y2 = lastOutputY;

    if (startComp != nullptr)
    {
        Point<int> startPos = startComp->getPinPos();
        x1 = startPos.x;
        y1 = startPos.y;
    }

    if (endComp != nullptr)
    {
        Point<int> endPos = endComp->getPinPos();
        x2 = endPos.x;
        y2 = endPos.y;
    }
}

void LinkComponent::changeListenerCallback (ChangeBroadcaster*)
{
    const bool nowSelected = owner.getSelectedObjects().isSelected (this);

    if (selected != nowSelected)
    {
        selected = nowSelected;
        repaint();
    }
    update();
}

bool LinkComponent::sameStartEnd(ValueTree linkTree)
{
    return linkTree.getProperty(Ids::startVertex) == data.getProperty(Ids::startVertex) 
        && linkTree.getProperty(Ids::endVertex) == data.getProperty(Ids::endVertex);
}

void LinkComponent::drawPath(Graphics& g)
{
    float x1 = curInputX;
    float y1 = curInputY;
    float x2 = curOutputX;
    float y2 = curOutputY;
    g.setColour(currentColor);
    if(data.getType() == Ids::link)
    {
        Path iconPath;
        iconPath = ResourceLoader::getInstance()->getPathForLinkId(Ids::link,
                                                                   0,
                                                                   0,
                                                                   iconWidth*0.7,
                                                                   iconHeight*0.7);
        float rotateVal;
        if(segmented)
        {
            linePath.clear();
            linePath.startNewSubPath(x1, y1);
            linePath.lineTo(x1, (y2 - y1)/2 + y1);
            linePath.lineTo(x2, (y2 - y1)/2 + y1);
            linePath.lineTo(x2, y2);

            rotateVal = float_Pi * (x2 - x1 < 0 ? -1.0f: 2.0f);
        }
        else
        {
            linePath.clear();
            linePath.startNewSubPath(x1, y1);
            linePath.lineTo(x2, y2);

            rotateVal = float_Pi * 0.5f - (float) atan2(x2 - x1, y2 - y1);
        }
        PathStrokeType wideStroke(8.0f);
        wideStroke.createStrokedPath(hitPath, linePath);

        PathStrokeType stroke(1.5f);
        stroke.createStrokedPath(linePath, linePath);

        iconPath.applyTransform(AffineTransform::identity
                                .rotated(rotateVal)
                                .translated((x1 + x2) * 0.5f,
                                            (y1 + y2) * 0.5f));

        PathStrokeType stroke2(0.4f);
        stroke2.createStrokedPath(iconPath, iconPath);
        linePath.addPath(iconPath);
    }
    else if(data.getType() == Ids::pluck)
    {
        if (x1 == x2 && y1 == y2)
        {
            x2 = x1 + 1;
            y2 = y2 + 1;
        }
        Path iconPath;
        iconPath = ResourceLoader::getInstance()->getPathForLinkId(Ids::pluck,
                                                                   0,
                                                                   0,
                                                                   iconWidth,
                                                                   iconHeight / 2);
        float rotateVal;
        if(segmented)
        {
            linePath.clear();
            linePath.startNewSubPath(x1, y1);

            if(abs(x2 - x1) < iconWidth)
            {
                linePath.lineTo(x1, (y2 - y1) / 2 + y1 - iconHeight / 4);
                linePath.startNewSubPath(x2, (y2 - y1) / 2 + y1 + iconHeight / 4);
            }
            else
            {
                linePath.lineTo(x1, (y2 - y1) / 2 + y1);
                if (x2 - x1 > 0)
                {
                    linePath.lineTo(x1 + ((x2 - x1) / 2) - iconWidth / 2, (y2 - y1) / 2 + y1);
                    linePath.startNewSubPath(x1 + ((x2 - x1) / 2) + iconWidth / 2, (y2 - y1) / 2 + y1);
                }
                else
                {
                    linePath.lineTo(x1 + ((x2 - x1) / 2) + iconWidth / 2, (y2 - y1) / 2 + y1);
                    linePath.startNewSubPath(x1 + ((x2 - x1) / 2) - iconWidth / 2, (y2 - y1) / 2 + y1);
                }
                linePath.lineTo(x2, (y2 - y1) / 2 + y1);
            }
            linePath.lineTo(x2, y2);

            rotateVal = float_Pi * (x2 - x1 < 0 ? -1.0f : 2.0f);
        }
        else
        {
            //This is the little bit of the path which covers the icon
            float c = (iconWidth / 2.0f) / sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));

            linePath.clear();
            linePath.startNewSubPath(x1, y1);
            linePath.lineTo(x1 + (x2 - x1) * (0.5f - c),
                            y1 + (y2 - y1) * (0.5f - c));

            linePath.startNewSubPath(x1 + (x2 - x1)*(0.5f + c),
                                     y1 + (y2 - y1)*(0.5f + c));
            linePath.lineTo(x2, y2);

            rotateVal = float_Pi * 0.5f - (float) atan2(x2 - x1, y2 - y1);
            
        }
        PathStrokeType wideStroke(8.0f);
        wideStroke.createStrokedPath(hitPath, linePath);

        PathStrokeType stroke(1.5f);
        stroke.createStrokedPath(linePath, linePath);

        iconPath.applyTransform(AffineTransform::identity
                                .rotated(rotateVal)
                                .translated((x1 + x2) * 0.5f,
                                            (y1 + y2) * 0.5f));
        PathStrokeType stroke2(1.4f);
        stroke2.createStrokedPath(iconPath, iconPath);
        linePath.addPath(iconPath);
    }
    else if(data.getType() == Ids::touch ||
            data.getType() == Ids::pulsetouch)
    {
        if (x1 == x2 && y1 == y2)
        {
            x2 = x1 + 1;
            y2 = y2 + 1;
        }
        Path iconPath;
        iconPath = ResourceLoader::getInstance()->getPathForLinkId(data.getType(),
                                                                   0,
                                                                   0,
                                                                   iconWidth,
                                                                   iconHeight);
       
        float rotateVal;
        if (segmented)
        {
            linePath.clear();
            linePath.startNewSubPath(x1, y1);

            if(abs(x2 - x1) < iconWidth)
            {
                linePath.lineTo(x1, (y2 - y1) / 2 + y1 - iconHeight / 2);
                linePath.startNewSubPath(x2, (y2 - y1) / 2 + y1 + iconHeight / 2);
            }
            else
            {
                linePath.lineTo(x1, (y2 - y1) / 2 + y1);
                if (x2 - x1 > 0)
                {
                    linePath.lineTo(x1 + ((x2 - x1) / 2) - iconWidth / 2, (y2 - y1) / 2 + y1);
                    linePath.startNewSubPath(x1 + ((x2 - x1) / 2) + iconWidth / 2, (y2 - y1) / 2 + y1);
                }
                else
                {
                    linePath.lineTo(x1 + ((x2 - x1) / 2) + iconWidth / 2, (y2 - y1) / 2 + y1);
                    linePath.startNewSubPath(x1 + ((x2 - x1) / 2) - iconWidth / 2, (y2 - y1) / 2 + y1);
                }
                linePath.lineTo(x2, (y2 - y1) / 2 + y1);
            }
            linePath.lineTo(x2, y2);

            rotateVal = float_Pi * (x2 - x1 < 0 ? -1.0f : 2.0f);

        }
        else
        {
            //This is the little bit of the path which covers the icon
            float c = (iconWidth / 2.0f) / sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));

            linePath.clear();
            linePath.startNewSubPath(x1, y1);
            linePath.lineTo(x1 + (x2 - x1) * (0.5f - c),
                            y1 + (y2 - y1) * (0.5f - c));

            linePath.startNewSubPath((x1 + (x2 - x1) *(0.5f + c)),
                                     (y1 + (y2 - y1) *(0.5f + c)));
            linePath.lineTo(x2, y2);

            rotateVal = float_Pi * 0.5f - (float) atan2(x2 - x1, y2 - y1);

        }
        PathStrokeType wideStroke(8.0f);
        wideStroke.createStrokedPath(hitPath, linePath);

        PathStrokeType stroke(1.5f);
        stroke.createStrokedPath(linePath, linePath);

        iconPath.applyTransform(AffineTransform::identity
                                .rotated(rotateVal)
                                .translated((x1 + x2) * 0.5f,
                                            (y1 + y2) * 0.5f));
        linePath.addPath(iconPath);

        if(data.getType() == Ids::pulsetouch)
        {
            Colour c = currentColor;
            g.setColour(Colours::indigo);
            
            Path outlineRect;

            outlineRect.addRectangle(0 - (iconWidth * 0.15),
                                     0 - (iconHeight * 0.15),
                                     iconWidth + (iconWidth * 0.3),
                                     iconHeight + (iconHeight * 0.3));

            outlineRect.applyTransform(AffineTransform::translation((-iconWidth/2),
                                                                    -iconHeight/2));
            outlineRect.applyTransform(AffineTransform::identity
                                    .rotated(rotateVal)
                                    .translated((x1 + x2) * 0.5f,
                                                (y1 + y2) * 0.5f));
            PathStrokeType stroke(6.0f);
            stroke.createStrokedPath(outlineRect, outlineRect);
            g.fillPath(outlineRect);

            g.setColour(c);
        }
   }
    else if(data.getType() == Ids::waveguide)
    {
        linePath.clear();
        linePath.startNewSubPath(x1, y1);
        linePath.lineTo(x2, y2);

        PathStrokeType wideStroke(8.0f);
        wideStroke.createStrokedPath(hitPath, linePath);

        PathStrokeType stroke(5.0f);
        stroke.createStrokedPath(linePath, linePath);
    }
    linePath.setUsingNonZeroWinding(true);

    g.fillPath(linePath);
}

void LinkComponent::reverseDirection()
{
    String tempStart = data.getProperty(Ids::startVertex);
    String tempEnd = data.getProperty(Ids::endVertex);
    
    data.setProperty(Ids::startVertex, tempEnd, nullptr);
    data.setProperty(Ids::endVertex, tempStart, nullptr);
    
    startComp = owner.getObjectForId(tempEnd);
    endComp = owner.getObjectForId(tempStart);

    update();
}

Rectangle<int> LinkComponent::getIntersectioBounds()
{
    const Rectangle<int> intersectionBounds((int) jmin(lastInputX, lastOutputX),
                                            (int) jmin(lastInputY, lastOutputY),
                                            (int) fabsf(lastInputX - lastOutputX),
                                            (int) fabsf(lastInputY - lastOutputY));
    return intersectionBounds;
}

Point<int> LinkComponent::getPinPos()
{
    return getIntersectioBounds().getCentre();
}