/*
  ==============================================================================

    MDLParser_test.h
    Created: 20 Sep 2012 4:18:11pm
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

#ifndef __MDLPARSER_TEST_H_E5DA3686__
#define __MDLPARSER_TEST_H_E5DA3686__

#include "TestUtils.h"

namespace synthamodeler
{
class MDLParserTest : public UnitTest
{
public:
    MDLParserTest() : UnitTest("MDLParserTest") {}

    void runTest()
    {
        beginTest("Test regular expressions");

        RegularExpression re;
        // test 1
        expect(re.fullMatch(SAMRegex::getVertexLine(),
                            "mass(0.003,0.0,0.0),bounceme; # pos 100, 100"));
        // test 2
        expect(re.fullMatch(SAMRegex::getVertexLine(),
                            "mass(0.003,0.0,0.0),bounceme;"));
        // test 3
        expect(re.fullMatch(SAMRegex::getLinkLine(),
                            "link(3200.0, 2.5, 0.00),touch2 , bounceme, dev1;"));
        // test 4
        expect(re.fullMatch(SAMRegex::getAudioOutLine(),
                            "audioout,a3,bounceme*1.0; # pos 100, 100"));
        // test 5
        expect(re.fullMatch(SAMRegex::getAudioOutLine(),
                            "audioout,a3,bounceme*1.0;"));
        // test 6
        expect(re.fullMatch(SAMRegex::getAudioOutLine(),
                            "audioout,a3,(bounceme*1.0):filter;"));
        // test 7
        expect(re.fullMatch(SAMRegex::getFaustLine(),
                            "faustcode: frequencyScaler = 0.3;"));
        // test 8
        expect(re.fullMatch(SAMRegex::getTerminationLine(),
                            "termination(simpleStringTerm(-0.991,30) ),t1; #pos 100, 100"));
        // test 9
        expect(re.fullMatch(SAMRegex::getTerminationLine(),
                            "termination(simpleStringTerm(-0.991,30) ),t1;"));
        // test 10
        expect(re.fullMatch(SAMRegex::getJunctionLine(),
                            "junction(0.0),junct1; # pos 100, 100"));
        // test 11
        expect(re.fullMatch(SAMRegex::getJunctionLine(),
                            "junction(0.0),junct1;"));
        // test 12
        expect(re.fullMatch(SAMRegex::getWaveguideLine(),
                            "waveguide(0.5 ,simpleString(0.023, 0.023*frequencyScaler)),w4,t4, junct1;"));
        // test 13
        expect(re.fullMatch(SAMRegex::getVertexLine(),
                            "port(),dev1;"));
        // test 14
        expect(re.fullMatch(SAMRegex::getVertexLine(),
                            "port(),dev1; # pos 100, 100"));
        // test 15
        expect(re.fullMatch(SAMRegex::getVertexLine(),
                            "ground(0.0),g; # pos 100, 100"));
        // test 16
        expect(re.fullMatch(SAMRegex::getVertexLine(),
                            "ground(0.0),g;"));
        // test 17
        expect(re.fullMatch(SAMRegex::getVertexLine(),
                            "resonators(200.0,1.5,0.01),r0; # pos 223,385"));
        // test 18
        expect(re.fullMatch(SAMRegex::getVertexLine(),
                            "resonators(200.0,1.5,0.01),r0;"));
        // test 19
        expect(re.fullMatch(SAMRegex::getParamsLine(3),
                            "200.0,1.5,0.01"));
        // test 20
        expect(re.fullMatch(SAMRegex::getParamsLine(3),
                            "200.0*adjStiffness,1.5,0.01"));
        // test 21
        expect(re.fullMatch(SAMRegex::getParamsLine(2),
                            "8.0,simpleString(1.0,0.001)"));
        // test 22
        expect(re.fullMatch(SAMRegex::getParamsLine(1),
                            "simpleString(1.0,0.001,0.1)"));
        // test 23
        expect(re.fullMatch(SAMRegex::getParamsLine(1),
                            "1.0"));
        // test 24
        expect(re.fullMatch(SAMRegex::getParamsLine(2),
                            "simpleString(1.0, 0.001,3.0) , 8.0"));
        // test 25
        StringArray paramsArray;
        expect(re.fullMatchValues(SAMRegex::getParamsLine(3),
                                  "(1.0 + randomTweak * (-0.799939)),0.0,0.0",
                                  paramsArray, 3));
        // test 26
        expect(paramsArray[0].compare("(1.0 + randomTweak * (-0.799939))") == 0 &&
               paramsArray[1].compare("0.0") == 0 &&
               paramsArray[2].compare("0.0") == 0);
//        for (int i = 0; i < paramsArray.size(); ++i) {
//            DBG(paramsArray[i]);
//        }
        // test 27
        expect(re.partialMatch("\\A\\s*(faustcode):.*",
                               "faustcode: adjStiffness=hslider(\"stiffness\", 2200.0, 500.0, 100.0, 4000.0);"));
        // test28
        StringArray vals;
        expect(re.fullMatchValues(SAMRegex::getVertexLine(),
                                  "mass(0.003,0.0,0.0),mass;",
                                  vals, 3));
        // test 29
        expect(re.fullMatch(SAMRegex::params, "0.5, simpleString(0.033, 0.033*frequencyScaler)"));

        // test 30
        StringArray vals2;
        expect(re.fullMatchValues(SAMRegex::getWaveguideLine(), "waveguide(0.5, simpleString(0.033, 0.033*frequencyScaler)),w3,t3 ,junct1;",
                                  vals2, 5));
        // test 31
        expect(vals2[0].compare("waveguide") == 0 &&
               vals2[1].compare("0.5, simpleString(0.033, 0.033*frequencyScaler)") == 0 &&
               vals2[2].compare("w3") == 0 &&
               vals2[3].compare("t3") == 0 &&
               vals2[4].compare("junct1") == 0);
        // test 32
        StringArray vals3;
        expect(re.fullMatchValues(SAMRegex::getVertexLine(),
                                  "mass(massMembrane*(1.0 + randomTweak*(-0.799939)),0.0,0.0),mn0xn20y;",
                                  vals3, 4));
        // test 33
        expect(vals3[0].compare("mass") == 0 &&
               vals3[1].compare("massMembrane*(1.0 + randomTweak*(-0.799939)),0.0,0.0") == 0 &&
               vals3[2].compare("mn0xn20y") == 0 &&
               vals3[3].compare("") == 0);

        // test 34
        StringArray vals4;
        expect(re.fullMatchValues(SAMRegex::getParamsLine(3),
                                  "massMembrane*(1.0 + randomTweak*(-0.799939)),0.0,0.0",
                                  vals4, 3));
        // test 35
        expect(vals4[0].compare("massMembrane*(1.0 + randomTweak*(-0.799939))") == 0 &&
               vals4[1].compare("0.0") == 0 &&
               vals4[2].compare("0.0") == 0);

        // test 36
        expect(re.fullMatch(SAMRegex::getCommentObjectLine(),
                            "## comment(\"Test\", 16, ff000000),c1; # pos 200, 100"));

        // test 37
        StringArray commentVals;
        expect(re.fullMatchValues(SAMRegex::getCommentObjectLine(),
                                  "## comment(\"Test\", 16, ff000000),comment1; # pos 200, 100",
                                  commentVals, 4));

        // test 38
        expect(commentVals[0].compare("comment") == 0 &&
               commentVals[1].compare("\"Test\", 16, ff000000") == 0 &&
               commentVals[2].compare("comment1") == 0 &&
               commentVals[3].compare("# pos 200, 100") == 0);

        // test 39
        StringArray commentParams;
        expect(re.fullMatchValues(SAMRegex::getParamsLine(3),
                                  "\"Test\", 16, ff000000",
                                  commentParams, 3));

        // test 40
        expect(commentParams[0].removeCharacters("\\\"").compare("Test") == 0 &&
               commentParams[1].compare("16") == 0 &&
               commentParams[2].compare("ff000000") == 0);

        // test 41
        StringArray resonatorsParams;
        String reResStr;
        reResStr << "\\s*" << SAMRegex::paramsDetailRes << "\\s*[,]*";
        RegularExpression reResonators(reResStr);
        reResonators.findAndConsume("0.001, 0.3, 0.1,0.001, 0.3, 0.1,200.0,decayTime*1.0,0.01",
                                    resonatorsParams);
        expect(resonatorsParams.size() == 9);


    }
};

static MDLParserTest mdlParserTest;
}

#endif  // __MDLPARSER_TEST_H_E5DA3686__
