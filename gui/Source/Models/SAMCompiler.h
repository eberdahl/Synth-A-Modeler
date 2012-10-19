/*
  ==============================================================================

    SAMCompiler.h
    Created: 19 Oct 2012 3:48:21pm
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

#ifndef __SAMCOMPILER_H_7CDF5BCC__
#define __SAMCOMPILER_H_7CDF5BCC__


namespace synthamodeler
{

static const char* wgL = "L";
static const char* wgR = "R";
static const char* wgLp = "Lp";
static const char* wgRp = "Rp";
static const char* jTO = "TO";
static const char* jOutputs = "Outputs";

class SAMCompiler
{
private:
    SAMCompiler();
    ~SAMCompiler();

    struct MassLinkRef
    {
        String massId;
        StringArray linkRefs;
        bool isPort;
    };
    
    static int containsMassLinkRef(const OwnedArray<MassLinkRef>& mlf, const String& mId)
    {
        for (int i = 0; i < mlf.size(); ++i)
        {
            MassLinkRef* m = mlf[i];
            if (m->massId.compare(mId) == 0)
                return i;
        }
        return -1;
    }

    static ValueTree getJunctionLink(ValueTree linksTree, ValueTree jnctTree)
    {
        for (int i = 0; i < linksTree.getNumChildren(); ++i)
        {
            ValueTree link = linksTree.getChild(i);
            if (link[Ids::startVertex] == jnctTree[Ids::identifier]
                || link[Ids::endVertex] == jnctTree[Ids::identifier])
                return link;
        }
        return ValueTree::invalid;
    }
public:

    static const String compile(ValueTree mdlRoot_)
    {
        String dspContent;

        dspContent << "// This DSP file has been generated by the Synth-A-Modeler compiler.\n";
        dspContent << "import(\"physicalmodeling.lib\");\n\n";

        ValueTree faustTree = mdlRoot_.getChildWithName(Objects::variables);
        ValueTree massTree = mdlRoot_.getChildWithName(Objects::masses);
        ValueTree linkTree = mdlRoot_.getChildWithName(Objects::links);
        ValueTree wgTree = mdlRoot_.getChildWithName(Objects::waveguides);
        ValueTree jTree = mdlRoot_.getChildWithName(Objects::junctions);
        ValueTree tTree = mdlRoot_.getChildWithName(Objects::terminations);
        ValueTree aoTree = mdlRoot_.getChildWithName(Objects::audioobjects);

        int numMasslike = 0;
        int numPorts = 0;
        int numWaveguides = 0;
        String wgTermString;
        String junctString;
        StringArray wgOutputs;
        StringArray wgInputs;
        StringArray massWithJunct;
        StringArray linkWithJunct;
        StringArray massWithJunctLine;
        StringArray massWithJunctOutputs;
        for (int i = 0; i < wgTree.getNumChildren(); ++i)
        {
            ++numWaveguides;
            ValueTree wg = wgTree.getChild(i);
            ValueTree left = jTree.getChildWithProperty(Ids::identifier,
                                                        wg[Ids::startVertex]);
            ValueTree right;
            if (left.isValid())
            {
                right = tTree.getChildWithProperty(Ids::identifier,
                                                   wg[Ids::endVertex]);
            }
            else
            {
                left = tTree.getChildWithProperty(Ids::identifier, wg[Ids::startVertex]);
                right = jTree.getChildWithProperty(Ids::identifier, wg[Ids::endVertex]);
            }
            ValueTree term;
            ValueTree junct;
            StringArray wgSuffixes;
            if (left.getType() == Ids::termination)
            {
                wgSuffixes.add(wgR);
                wgSuffixes.add(wgL);
                wgSuffixes.add(wgRp);
                wgSuffixes.add(wgLp);
                term = left;
                junct = right;
            }
            else if (left.getType() == Ids::junction)
            {
                wgSuffixes.add(wgL);
                wgSuffixes.add(wgR);
                wgSuffixes.add(wgLp);
                wgSuffixes.add(wgRp);
                term = right;
                junct = left;
            }

            wgInputs.add(wg[Ids::identifier].toString() + wgLp);
            wgInputs.add(wg[Ids::identifier].toString() + wgRp);
            wgOutputs.add(wg[Ids::identifier].toString() + wgL);
            wgOutputs.add(wg[Ids::identifier].toString() + wgR);

            wgTermString << "\t";
            wgTermString << wg[Ids::identifier].toString();
            wgTermString << wgSuffixes[0] << " = " << term[Ids::identifier].toString();

            ValueTree paWg = wg.getChildWithName(Ids::parameters);
            StringArray paWgStrings;
            for (int j = 0; j < paWg.getNumChildren(); ++j)
            {
                paWgStrings.add(paWg.getChild(j)[Ids::value].toString());
            }

            wgTermString << " : ";
            wgTermString << paWgStrings[1];
            wgTermString << ";\n\t";

            wgTermString << term[Ids::identifier].toString();
            wgTermString << " = ";
            wgTermString << wg[Ids::identifier].toString() << wgSuffixes[3];
            wgTermString << " : ";

            ValueTree paTerm = term.getChildWithName(Ids::parameters);
            StringArray paTermStrings;
            for (int j = 0; j < paTerm.getNumChildren(); ++j)
            {
                paTermStrings.add(paTerm.getChild(j)[Ids::value].toString());
            }

            wgTermString << paTermStrings[0] << ";\n\n";

            junctString << "\t";
            junctString << wg[Ids::identifier].toString() << wgSuffixes[1];
            junctString << " = ";
            junctString << junct[Ids::identifier].toString() << jTO;
            junctString << wg[Ids::identifier].toString();
            junctString << " : ";
            junctString << paWgStrings[1];
            junctString << ";\n\t";

            junctString << junct[Ids::identifier].toString() << jTO;
            junctString << wg[Ids::identifier].toString() << " = ";
            junctString << junct[Ids::identifier].toString() << jOutputs;
            junctString << ":(_,!)-";
            junctString << wg[Ids::identifier].toString() << wgSuffixes[2];
            junctString << ";\n\t";

            String junctLinkString;
            String junctMassString;
            // Check if junction has one link connected
            ValueTree junctLink = getJunctionLink(linkTree, junct);
            if (junctLink.isValid())
            {
                String jm;
                if (junctLink[Ids::startVertex] == junct[Ids::identifier])
                    jm << junctLink[Ids::endVertex].toString();
                else
                    jm << junctLink[Ids::startVertex].toString();
                junctMassString << jm << "p";

                massWithJunct.add(jm);
                massWithJunctOutputs.add(jm + "p");
                linkWithJunct.add(junctLink[Ids::identifier].toString());

                ValueTree junctLinkParams = junctLink.getChildWithName(Ids::parameters);
                StringArray junctLinkParamsStrings;
                for (int k = 0; k < junctLinkParams.getNumChildren(); ++k)
                {
                    ValueTree param = junctLinkParams.getChild(k);
                    junctLinkParamsStrings.add(param[Ids::value].toString());
                }
                junctLinkString << "junction" << junctLink.getType().toString();
                junctLinkString << "Underneath(0.0,";
                junctLinkString << junctLinkParamsStrings.joinIntoString(",");
                junctLinkString << ")";

                // Get mass-like object connected with junction > link
                ValueTree mwj = massTree.getChildWithProperty(Ids::identifier, jm);
                String mwjl = "\t";
                mwjl << jm;
                mwjl << " = (0.0+(";
                mwjl << junct[Ids::identifier].toString() << jOutputs << ":(!,_)))";
                mwjl << " : ";
                ValueTree mwjp = mwj.getChildWithName(Ids::parameters);
                StringArray mwjpStrings;
                for (int p = 0; p < mwjp.getNumChildren(); ++p)
                {
                    ValueTree param = mwjp.getChild(p);
                    mwjpStrings.add(param[Ids::value].toString());
                }
                mwjl << mwj.getType().toString();
                mwjl << "(" << mwjpStrings.joinIntoString(",") << ");";
                massWithJunctLine.add(mwjl);

            }
            else
            {
                junctMassString << "0.0";
                junctLinkString << "junctionlink(0.0, 0.0, 0.0, 0.0)";
            }
            junctString << junct[Ids::identifier].toString() << jOutputs;
            junctString << " = (";
            junctString << junctMassString << ", 0.0+";
            junctString << wg[Ids::identifier].toString() << wgSuffixes[2];
            junctString << "*" << paWgStrings[0];
            junctString << ", 0.0+" << paWgStrings[0];
            junctString << ") : ";
            junctString << junctLinkString << ";\n\t";

            junctString << junct[Ids::identifier].toString();
            junctString << " = ";
            junctString << junct[Ids::identifier].toString() << jOutputs;
            junctString << ":(_,!);\n\n";

        }

        // Write all faustcode
        for (int i = 0; i < faustTree.getNumChildren(); ++i)
        {
            ValueTree fa = faustTree.getChild(i);
            dspContent << fa[Ids::identifier].toString();
            dspContent << "=";
            dspContent << fa[Ids::faustCode].toString();
            dspContent << ";\n";
        }

        dspContent << "\n";

        // Get all mass names
        OwnedArray<MassLinkRef> massLinkRefs;
        for (int i = 0; i < massTree.getNumChildren(); ++i)
        {
            ValueTree ma = massTree.getChild(i);
            if (massWithJunct.contains(ma[Ids::identifier].toString()))
                continue;
            MassLinkRef* mlf = new MassLinkRef();
            mlf->massId = ma[Ids::identifier].toString();
            StringArray mlfa;
            mlf->linkRefs = mlfa;
            if (ma.getType() == Ids::port)
                mlf->isPort = true;
            else
                mlf->isPort = false;
            massLinkRefs.add(mlf);
        }

        // Write all link-like objects
        StringArray linkobjects;
        for (int i = 0; i < linkTree.getNumChildren(); ++i)
        {
            ValueTree li = linkTree.getChild(i);
            String linkId = li[Ids::identifier].toString();
            if (linkWithJunct.contains(linkId))
                continue;
            String startVertex = li[Ids::startVertex].toString();
            String endVertex = li[Ids::endVertex].toString();
            int sIdx = containsMassLinkRef(massLinkRefs, startVertex);
            if (sIdx >= 0)
                massLinkRefs[sIdx]->linkRefs.add("-" + linkId);

            int eIdx = containsMassLinkRef(massLinkRefs, endVertex);
            if (eIdx >= 0)
                massLinkRefs[eIdx]->linkRefs.add("+" + linkId);

            String tagName = li.getType().toString();
            ValueTree params = li.getChildWithName(Ids::parameters);
            StringArray paramsStr;
            for (int k = 0; k < params.getNumChildren(); ++k)
            {
                ValueTree param = params.getChild(k);
                paramsStr.add(param[Ids::value].toString());
            }

            String linkLine;
            linkLine << "\t";
            linkLine << linkId;
            linkLine << " = (";

            linkLine << startVertex << "p - ";
            linkLine << endVertex << "p) : ";
            linkLine << tagName << "(";
            linkLine << paramsStr.joinIntoString(",") << ");";
            linkobjects.add(linkLine);
        }

        StringArray massobjects;

        // write all mass-like object except those connected to junctions
        for (int i = 0; i < massTree.getNumChildren(); ++i)
        {
            ValueTree ma = massTree.getChild(i);
            if (massWithJunct.contains(ma[Ids::identifier].toString()))
                continue;

            ++numMasslike;
            String tagName = ma.getType().toString();
            String massName = ma[Ids::identifier].toString();
            String massLine;
            massLine << "\t";
            massLine << massName << " = (0.0";
            if (tagName.compare("port") == 0)
                ++numPorts;

            int mIdx = containsMassLinkRef(massLinkRefs, massName);
            if (mIdx >= 0)
            {
                if (massLinkRefs[mIdx]->linkRefs.size() > 0)
                {
                    massLine << massLinkRefs[mIdx]->linkRefs.joinIntoString(String::empty);
                }
            }

            massLine << ")";
            if (tagName.compare("port") != 0)
            {
                massLine << " : ";
                massLine << tagName << "(";
                ValueTree params = ma.getChildWithName(Ids::parameters);
                StringArray paramsStr;
                for (int k = 0; k < params.getNumChildren(); ++k)
                {
                    ValueTree param = params.getChild(k);
                    paramsStr.add(param[Ids::value].toString());
                }
                massLine << paramsStr.joinIntoString(",") << ")";
            }
            massLine << ";";
            massobjects.add(massLine);

        }
        // add remaining mass-like object which are connected to junctions
        massobjects.addArray(massWithJunctLine);

        StringArray audioobjects;
        StringArray audioNames;
        for (int i = 0; i < aoTree.getNumChildren(); ++i)
        {
            ValueTree ao = aoTree.getChild(i);

            String audioLine;
            String audioName = ao[Ids::identifier].toString();
            audioNames.add(audioName);
            audioLine << "\t";
            audioLine << audioName << " = ";
            ValueTree sources = ao.getChildWithName(Ids::sources);
            StringArray paramsStr;
            if (sources.getNumChildren() > 0)
            {
                for (int k = 0; k < sources.getNumChildren(); ++k)
                {
                    ValueTree src = sources.getChild(k);
                    paramsStr.add(src[Ids::value].toString());
                }
                audioLine << paramsStr.joinIntoString("+");
            }

            String optional = ao[Ids::optional].toString();
            if (optional != String::empty)
                audioLine << " : " << optional;
            audioLine << ";";
            audioobjects.add(audioLine);
        }

        StringArray inputs;
        StringArray inputsPorts;
        for (int i = 0; i < massLinkRefs.size(); ++i)
        {
            if (massLinkRefs[i]->isPort)
                inputsPorts.add(massLinkRefs[i]->massId);
            else
                inputs.add(massLinkRefs[i]->massId);
        }

        StringArray outputs = inputs;
        outputs.addArray(massWithJunct);
        StringArray outputsPorts = inputsPorts;

        StringArray inputsP;
        StringArray inputsPPorts;
        for (int i = 0; i < inputs.size(); ++i)
        {
            String inputP = inputs[i];
            inputP << "p";
            inputsP.add(inputP);
        }
        for (int i = 0; i < massWithJunctOutputs.size(); ++i)
        {
            ++numMasslike;
            String inputP = massWithJunctOutputs[i];
            inputsP.add(inputP);
        }
        for (int i = 0; i < inputsPorts.size(); ++i)
        {
            String inputPPort = inputsPorts[i];
            inputPPort << "p";
            inputsPPorts.add(inputPPort);
        }

        dspContent << "bigBlock(" << inputsP.joinIntoString(",");
        if (wgInputs.size() > 0)
            dspContent << "," << wgInputs.joinIntoString(",");
        if (inputsPPorts.size() > 0)
            dspContent << "," << inputsPPorts.joinIntoString(",");
        dspContent << ") = (";
        dspContent << outputs.joinIntoString(",");
        if (wgOutputs.size() > 0)
            dspContent << "," << wgOutputs.joinIntoString(",");
        if (outputsPorts.size() > 0)
            dspContent << "," << outputsPorts.joinIntoString(",");
        if (audioNames.size() > 0)
            dspContent << "," << audioNames.joinIntoString(",");
        dspContent << ") with {\n";
        dspContent << "\n\t//waveguide termination objects\n";
        dspContent << wgTermString;
        dspContent << "\t//junctions\n";
        dspContent << junctString;
        dspContent << "\t//mass-like objects\n";
        dspContent << massobjects.joinIntoString("\n") << "\n";
        dspContent << "\n\t//link-like objects\n";
        dspContent << linkobjects.joinIntoString("\n") << "\n";
        dspContent << "\n\t//audio objects\n";
        dspContent << audioobjects.joinIntoString("\n") << "\n};\n\n";

        StringArray feedbackArray;
        StringArray outputArray;
        for (int i = 0; i < numMasslike - numPorts + 2 * numWaveguides; ++i)
        {
            feedbackArray.add("_");
            outputArray.add("!");
        }
        for (int i = 0; i < numPorts + audioNames.size(); ++i)
        {
            feedbackArray.add("!");
            outputArray.add("_");
        }
        dspContent << "process = (bigBlock)~(";
        dspContent << feedbackArray.joinIntoString(",");
        dspContent << "):(";
        dspContent << outputArray.joinIntoString(",") << ");";

        massLinkRefs.clear();

        return dspContent;
    }
};


}

#endif  // __SAMCOMPILER_H_7CDF5BCC__
