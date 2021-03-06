/*
  ==============================================================================

    AudioOutConnector.cpp
    Created: 27 Aug 2012 2:11:55pm
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
#include "../View/SelectableObject.h"
#include "../Graph/Node.h"
#include "BaseObjectComponent.h"
#include "ObjectComponent.h"
#include "LinkComponent.h"
#include "../Controller/ObjController.h"
#include "ObjectsHolder.h"

#include "AudioOutConnector.h"

using namespace synthamodeler;

class GainPanel  : public DialogWindow
{
public:
	GainPanel(AudioOutConnector* aoc, const String& sourceId, 
             ValueTree data, UndoManager* undoManager)
    : DialogWindow ("Gain", Colour::fromRGBA((uint8)200, (uint8)200, (uint8)200, (uint8)150), true)
    {
        GainComponent * const gc = new GainComponent(*this, sourceId, 
                                                     data, undoManager);
        gc->setSize(200, 100);

        setContentOwned(gc, true);

//        if (!restoreWindowStateFromString(variablesWindowPos))
//            centreWithSize(getWidth(), getHeight());
        centreAroundComponent(aoc, getWidth(), getHeight());
        setResizable(false, false);
        setVisible(true);
    }
    ~GainPanel()
    {
        
    }

    void closeButtonPressed()
    {
        setVisible (false);
    }

    static int show(AudioOutConnector* aoc, const String& sourceId, 
                    ValueTree data, UndoManager* undoManager)
    {
        GainPanel gp(aoc, sourceId, data, undoManager);
        gp.runModalLoop();
        return gp.returnVal;
    }

    int returnVal;
private:
    class GainComponent : public Component,
    					  public Button::Listener,
                          public TextEditor::Listener
    {
    public:
        GainComponent(GainPanel& parent_,
                      const String& sourceId_,
                      ValueTree data_,
                      UndoManager* undoManager_)
    	: parent(parent_),
    	  labelGain("Gain", "Gain"),
    	  teGain("Gain value"),
          sourceId(sourceId_),
    	  data(data_),
    	  btOk("Ok"),
    	  btCancel("Cancel"),
    	  undoManager(undoManager_)
        {
            ValueTree sources = data.getChildWithName(Ids::sources);
//            ValueTree source = sources.getChildWithProperty(Ids::value, sourceId);
//            oldGain = source[Ids::gain].toString();
            oldGain = Utils::getGainForSourceId(sources, sourceId);
            teGain.setText(oldGain);
            teGain.addListener(this);
    		addAndMakeVisible(&teGain);
            labelGain.attachToComponent(&teGain, true);
    		btOk.addListener(this);
    		addAndMakeVisible(&btOk);
    		btCancel.addListener(this);
    		addAndMakeVisible(&btCancel);
    	}

    	~GainComponent()
    	{
    	}

    	void resized()
    	{
    		labelGain.setBounds(0, 5, 60, 22);
    		teGain.setBounds(60, 5, getWidth() - 65, 22);
    		btOk.setBounds(getWidth()/2 - 65, getHeight() - 30, 60, 22);
    		btCancel.setBounds(getWidth()/2 + 5, getHeight() - 30, 60, 22);
    	}

    	void buttonClicked(Button* button)
    	{
            parent.returnVal = 0;
    		if(button == &btOk)
    		{
                applyEditing();
    		}
    		else if(button == &btCancel)
    		{
                cancelEditing();
    		}
        }
        void textEditorTextChanged(TextEditor& editor)
        {
        }
        void textEditorReturnKeyPressed(TextEditor& editor)
        {
            applyEditing();
        }
        void textEditorEscapeKeyPressed(TextEditor& editor)
        {
            cancelEditing();
        }
        void textEditorFocusLost(TextEditor& editor)
        {
        }
        void cancelEditing()
        {
            parent.returnVal = 2;
            parent.closeButtonPressed();
        }
        void applyEditing()
        {
//            ValueTree sources = data.getChildWithName(Ids::sources);
//            ValueTree source = sources.getChildWithProperty(Ids::value, sourceId);
            String newGain = teGain.getText();
            if (oldGain.compare(newGain) != 0)
            {
                undoManager->beginNewTransaction("Edit gain");
                ValueTree sources = data.getChildWithName(Ids::sources);
                Utils::setGainForSourceId(sources, sourceId, newGain, undoManager);
//                source.setProperty(Ids::gain, newGain, undoManager);
                undoManager->beginNewTransaction();
            }
            parent.returnVal = 1;
            parent.closeButtonPressed();
        }
    private:
    	GainPanel& parent;
    	Label labelGain;
    	TextEditor teGain;
        String sourceId;
    	ValueTree data;
    	TextButton btOk;
    	TextButton btCancel;
    	UndoManager* undoManager;
        String oldGain;
    };
};

AudioOutConnector::AudioOutConnector(ObjController& owner_, 
                                     BaseObjectComponent* objComp_,
                                     ObjectComponent* audioOutComp_)
    :
    owner(owner_),
    mouseDownSelectStatus(false),
    sourceComp(objComp_),
    audioOutComp(audioOutComp_)
{
//    audioOutComp
//    ValueTree auSources = audioOutComp->getData().getChildWithName(Ids::sources);
//    ValueTree source(Ids::audiosource);
//    source.setProperty(Ids::value, objComp->getData()[Ids::identifier], nullptr);
//    source.setProperty(Ids::gain, "1.0", nullptr);
//    auSources.addChild(source, -1, nullptr);

    segmented = StoredSettings::getInstance()->getIsSegmentedConnectors();
    owner.getSelectedObjects().addChangeListener(this);
//    owner.getSelectedAudioConnections().addChangeListener(this);
}

AudioOutConnector::~AudioOutConnector()
{
    owner.getSelectedObjects().removeChangeListener(this);
}

void AudioOutConnector::resized()
{
    float x1, y1, x2, y2;
    getPoints(x1, y1, x2, y2);

    lastInputX = x1;
    lastInputY = y1;
    lastOutputX = x2;
    lastOutputY = y2;

    x1 -= getX();
    y1 -= getY();
    x2 -= getX();
    y2 -= getY();

    float rotatVal;
    linePath.clear();
    if(segmented)
    {
        linePath.clear();
        linePath.startNewSubPath(x1, y1);
        linePath.lineTo(x1, (y2 - y1) / 2 + y1);
        linePath.lineTo(x2, (y2 - y1) / 2 + y1);
        linePath.lineTo(x2, y2);

        rotatVal = float_Pi * (x2 - x1 < 0 ? -1.0f: 2.0f);
    }
    else
    {
        linePath.startNewSubPath(x1, y1);
        linePath.lineTo(x2, y2);
        rotatVal = float_Pi * 0.5f - (float) atan2(x2 - x1, y2 - y1);
    }
    PathStrokeType wideStroke(8.0f);
    wideStroke.createStrokedPath(hitPath, linePath);

    float dashLengths[] = {5, 5};
    PathStrokeType stroke(1.5f);
    stroke.createDashedStroke(linePath, linePath, dashLengths, 2);
    //    stroke.createStrokedPath(linePath, linePath);
    const float arrowW = 7.0f;
    const float arrowL = 5.0f;

    Path arrow;
    arrow.addTriangle(-arrowL, arrowW,
                      -arrowL, -arrowW,
                      arrowL, 0.0f);

    arrow.applyTransform(AffineTransform::identity
                         .rotated(rotatVal)
                         .translated((x1 + x2) * 0.5f,
                                     (y1 + y2) * 0.5f));
    //    arrow.applyTransform(AffineTransform::translation((x2 - x1) * 0.3f, (y2 - y1) * 0.3f));

    linePath.addPath(arrow);

    linePath.setUsingNonZeroWinding(true);

}

void AudioOutConnector::paint(Graphics& g)
{
    if(! isVisible())
        return;

    if(selected)
    {
        g.setColour(Colours::red);
    }
    else
    {
        g.setColour(Colours::grey);
    }
    
    g.fillPath(linePath);
}

void AudioOutConnector::update()
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

void AudioOutConnector::resizeToFit()
{
    float x1, y1, x2, y2;
    getPoints(x1, y1, x2, y2);

    const Rectangle<int> newBounds((int) jmin(x1, x2) - 7,
                                   (int) jmin(y1, y2) - 7,
                                   (int) fabsf(x1 - x2) + 14,
                                   (int) fabsf(y1 - y2) + 14);

    if (newBounds != getBounds())
        setBounds(newBounds);
    else
        resized();

    repaint();
    
}

bool AudioOutConnector::hitTest(int x, int y)
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

void AudioOutConnector::getPoints(float& x1, float& y1, float& x2, float& y2) const
{
    x1 = lastInputX;
    y1 = lastInputY;
    x2 = lastOutputX;
    y2 = lastOutputY;

    if (sourceComp != nullptr)
    {
        Point<int> startPos;
        if(ObjectComponent* const oc = dynamic_cast<ObjectComponent*>(sourceComp))
            startPos = oc->getPinPos();
        else if(LinkComponent* const lc = dynamic_cast<LinkComponent*>(sourceComp))
            startPos = lc->getPinPos();
        x1 = startPos.x;
        y1 = startPos.y;
    }

    if (audioOutComp != nullptr)
    {
        Point<int> endPos = audioOutComp->getPinPos();
        x2 = endPos.x;
        y2 = endPos.y;
    }
}

void AudioOutConnector::changeListenerCallback (ChangeBroadcaster*)
{
    const bool nowSelected = owner.getSelectedObjects().isSelected (this);

    if (selected != nowSelected)
    {
        selected = nowSelected;
        repaint();
    }
    update();
}

void AudioOutConnector::mouseDown(const MouseEvent& e)
{
    mouseDownSelectStatus = owner.getSelectedObjects().addToSelectionOnMouseDown (this, e.mods);
}

void AudioOutConnector::mouseDrag(const MouseEvent& e)
{
    
}

void AudioOutConnector::mouseUp(const MouseEvent& e)
{
   	if (e.mouseWasClicked() && e.getNumberOfClicks() == 2)
	{
        showGainPanel();
	}

    owner.getSelectedObjects().addToSelectionOnMouseUp (this, e.mods, false,
                                                        mouseDownSelectStatus);
    update();
}

Rectangle<int> AudioOutConnector::getIntersectioBounds()
{
    const Rectangle<int> intersectionBounds((int) jmin(lastInputX, lastOutputX),
                                            (int) jmin(lastInputY, lastOutputY),
                                            (int) fabsf(lastInputX - lastOutputX),
                                            (int) fabsf(lastInputY - lastOutputY));
    return intersectionBounds;
}

void AudioOutConnector::showGainPanel()
{
    String sourceId;
    if(ObjectComponent* const oc = dynamic_cast<ObjectComponent*>(sourceComp))
        sourceId = oc->getData()[Ids::identifier].toString();
    else if(LinkComponent* const lc = dynamic_cast<LinkComponent*>(sourceComp))
        sourceId = lc->getData()[Ids::identifier].toString();
    
    GainPanel::show(this, sourceId, audioOutComp->getData(), owner.getUndoManager());
}