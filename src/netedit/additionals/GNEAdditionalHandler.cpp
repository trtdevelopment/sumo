/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEAdditionalHandler.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
// Builds trigger objects for netedit
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/geom/GeomConvHelper.h>
#include <utils/xml/XMLSubSys.h>
#include <netedit/changes/GNEChange_Additional.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>


#include "GNEAdditionalHandler.h"
#include "GNEBusStop.h"
#include "GNEAccess.h"
#include "GNECalibrator.h"
#include "GNECalibratorFlow.h"
#include "GNECalibratorRoute.h"
#include "GNECalibratorVehicleType.h"
#include "GNEChargingStation.h"
#include "GNEClosingLaneReroute.h"
#include "GNEClosingReroute.h"
#include "GNEContainerStop.h"
#include "GNEDestProbReroute.h"
#include "GNEDetectorE1.h"
#include "GNEDetectorE2.h"
#include "GNEDetectorE3.h"
#include "GNEDetectorEntry.h"
#include "GNEDetectorExit.h"
#include "GNEDetectorE1Instant.h"
#include "GNEParkingArea.h"
#include "GNEParkingSpace.h"
#include "GNERerouter.h"
#include "GNERerouterInterval.h"
#include "GNERouteProbReroute.h"
#include "GNEParkingAreaReroute.h"
#include "GNERouteProbe.h"
#include "GNEVaporizer.h"
#include "GNEVariableSpeedSign.h"
#include "GNEVariableSpeedSignStep.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEAdditionalHandler::GNEAdditionalHandler(const std::string& file, GNEViewNet* viewNet, bool undoAdditionals, GNEAdditional *additionalParent) :
    SUMOSAXHandler(file),
    myViewNet(viewNet),
    myUndoAdditionals(undoAdditionals),
    myAdditionalParent(additionalParent) {
    if(additionalParent) {
        myParentElements.insertElement(additionalParent->getTag());
        myParentElements.commitElementInsertion(additionalParent->getID());
    }
}


GNEAdditionalHandler::~GNEAdditionalHandler() {}


void
GNEAdditionalHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    // Obtain tag of element
    SumoXMLTag tag = static_cast<SumoXMLTag>(element);
    // only continue if tag is valid
    if(tag != SUMO_TAG_NOTHING) {
        // push element int stack
        myParentElements.insertElement(tag);
        // Call parse and build depending of tag
        switch (tag) {
            case SUMO_TAG_BUS_STOP:
                parseAndBuildBusStop(attrs, tag);
                break;
            case SUMO_TAG_TRAIN_STOP:
                parseAndBuildBusStop(attrs, SUMO_TAG_BUS_STOP);
                break;
            case SUMO_TAG_ACCESS:
                parseAndBuildAccess(attrs, tag);
                break;
            case SUMO_TAG_CONTAINER_STOP:
                parseAndBuildContainerStop(attrs, tag);
                break;
            case SUMO_TAG_CHARGING_STATION:
                parseAndBuildChargingStation(attrs, tag);
                break;
            case SUMO_TAG_E1DETECTOR:
            case SUMO_TAG_INDUCTION_LOOP:
                parseAndBuildDetectorE1(attrs, tag);
                break;
            case SUMO_TAG_E2DETECTOR:
            case SUMO_TAG_LANE_AREA_DETECTOR:
                parseAndBuildDetectorE2(attrs, tag);
                break;
            case SUMO_TAG_E3DETECTOR:
            case SUMO_TAG_ENTRY_EXIT_DETECTOR:
                parseAndBuildDetectorE3(attrs, tag);
                break;
            case SUMO_TAG_DET_ENTRY:
                parseAndBuildDetectorEntry(attrs, tag);
                break;
            case SUMO_TAG_DET_EXIT:
                parseAndBuildDetectorExit(attrs, tag);
                break;
            case SUMO_TAG_INSTANT_INDUCTION_LOOP:
                parseAndBuildDetectorE1Instant(attrs, tag);
                break;
            case SUMO_TAG_ROUTEPROBE:
                parseAndBuildRouteProbe(attrs, tag);
                break;
            case SUMO_TAG_VAPORIZER:
                parseAndBuildVaporizer(attrs, tag);
                break;
            case SUMO_TAG_VSS:
                parseAndBuildVariableSpeedSign(attrs, tag);
                break;
            case SUMO_TAG_STEP:
                parseAndBuildVariableSpeedSignStep(attrs, tag);
                break;
            case SUMO_TAG_CALIBRATOR:
                parseAndBuildCalibrator(attrs, tag);
                break;
            case SUMO_TAG_PARKING_AREA:
                parseAndBuildParkingArea(attrs, tag);
                break;
            case SUMO_TAG_PARKING_SPACE:
                parseAndBuildParkingSpace(attrs, tag);
                break;
            case SUMO_TAG_VTYPE:
                parseAndBuildCalibratorVehicleType(attrs, tag);
                break;
            case SUMO_TAG_ROUTE:
                parseAndBuildCalibratorRoute(attrs, tag);
                break;
            case SUMO_TAG_FLOW:
                parseAndBuildCalibratorFlow(attrs, tag);
                break;
            case SUMO_TAG_REROUTER:
                parseAndBuildRerouter(attrs, tag);
                break;
            case SUMO_TAG_INTERVAL:
                parseAndBuildRerouterInterval(attrs, tag);
                break;
            case SUMO_TAG_CLOSING_LANE_REROUTE:
                parseAndBuildRerouterClosingLaneReroute(attrs, tag);
                break;
            case SUMO_TAG_CLOSING_REROUTE:
                parseAndBuildRerouterClosingReroute(attrs, tag);
                break;
            case SUMO_TAG_DEST_PROB_REROUTE:
                parseAndBuildRerouterDestProbReroute(attrs, tag);
                break;
            case SUMO_TAG_PARKING_ZONE_REROUTE:
                parseAndBuildRerouterParkingAreaReroute(attrs, tag);
                break;
            case SUMO_TAG_ROUTE_PROB_REROUTE:
                parseAndBuildRerouterRouteProbReroute(attrs, tag);
                break;
            default:
                break;
        }
    }
}


void
GNEAdditionalHandler::myEndElement(int) {
    // pop last inserted element
    myParentElements.popElement();
}


void
GNEAdditionalHandler::parseAndBuildVaporizer(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of Vaporizer
    const std::string edgeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    double begin = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_BEGIN, abort);
    double end = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_END, abort);
    // Continue if all parameters were successfully loaded
    if (!abort) {
        // get GNEEdge
        GNEEdge* edge = myViewNet->getNet()->retrieveEdge(edgeID, false);
        // check that all parameters are valid
        if (edge == nullptr) {
            WRITE_WARNING("The edge '" + edgeID + "' to use within the " + toString(tag) + " is not known.");
        } else if (myViewNet->getNet()->retrieveAdditional(tag, edgeID, false) != nullptr) {
            WRITE_WARNING("There is already a " + toString(tag) + " in the edge '" + edgeID + "'.");
        } else if (begin > end) {
            WRITE_WARNING("Time interval of " + toString(tag) + " isn't valid. Attribute '" + toString(SUMO_ATTR_BEGIN) + "' is greater than attribute '" + toString(SUMO_ATTR_END) + "'.");
        } else if(buildVaporizer(myViewNet, myUndoAdditionals, edge, begin, end)) {
            // save ID of last created element
            myParentElements.commitElementInsertion(edgeID);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildRouteProbe(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of RouteProbe
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    std::string edgeId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_EDGE, abort);
    std::string freq = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_FREQUENCY, abort);
    std::string file = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_FILE, abort);
    double begin = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_BEGIN, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get edge
        GNEEdge* edge = myViewNet->getNet()->retrieveEdge(edgeId, false);
        // check that all elements are valid
        if (myViewNet->getNet()->retrieveAdditional(tag, id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + id + "'.");
        } else if (edge == nullptr) {
            // Write error if lane isn't valid
            WRITE_WARNING("The edge '" + edgeId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else {
            // Freq needs an extra check, because it can be empty
            if (GNEAttributeCarrier::canParse<double>(freq)) {
                if(GNEAttributeCarrier::parse<double>(freq) < 0) {
                    WRITE_WARNING(toString(SUMO_ATTR_FREQUENCY) + "of " + toString(tag) + "'" + id + "' cannot be negative.");
                    freq = "";
                }
            } else {
                if(freq.empty()) {
                    WRITE_WARNING(toString(SUMO_ATTR_FREQUENCY) + "of " + toString(tag) + "'" + id + "' cannot be parsed to float.");
                }
                freq = "";
            }
            if(buildRouteProbe(myViewNet, myUndoAdditionals, id, edge, freq, file, begin)) {
                // save ID of last created element
                myParentElements.commitElementInsertion(id);
            }
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildCalibratorRoute(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attribute of calibrator routes
    std::string routeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    std::string edgeIDs = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, routeID, tag, SUMO_ATTR_EDGES, abort);
    RGBColor color = GNEAttributeCarrier::parseAttributeFromXML<RGBColor>(attrs, routeID, tag, SUMO_ATTR_COLOR, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // obtain edges (And show warnings if isn't valid)
        std::vector<GNEEdge*> edges;
        if (GNEAttributeCarrier::canParse<std::vector<GNEEdge*> >(myViewNet->getNet(), edgeIDs, true)) {
            edges = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(myViewNet->getNet(), edgeIDs);
        }
        // check that all elements are valid
        if (myViewNet->getNet()->retrieveAdditional(SUMO_TAG_ROUTE, routeID, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + routeID + "'.");
        } else if ((edges.size() > 0) && buildCalibratorRoute(myViewNet, myUndoAdditionals, routeID, edges, color)) {
            // save ID of last created element
            myParentElements.commitElementInsertion(routeID);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildCalibratorVehicleType(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attribute of calibrator vehicle types
    std::string vehicleTypeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    double accel = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, vehicleTypeID, tag, SUMO_ATTR_ACCEL, abort);
    double decel = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, vehicleTypeID, tag, SUMO_ATTR_DECEL, abort);
    double sigma = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, vehicleTypeID, tag, SUMO_ATTR_SIGMA, abort);
    double tau = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, vehicleTypeID, tag, SUMO_ATTR_TAU, abort);
    double length = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, vehicleTypeID, tag, SUMO_ATTR_LENGTH, abort);
    double minGap = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, vehicleTypeID, tag, SUMO_ATTR_MINGAP, abort);
    double maxSpeed = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, vehicleTypeID, tag, SUMO_ATTR_MAXSPEED, abort);
    double speedFactor = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, vehicleTypeID, tag, SUMO_ATTR_SPEEDFACTOR, abort);
    double speedDev = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, vehicleTypeID, tag, SUMO_ATTR_SPEEDDEV, abort);
    RGBColor color = GNEAttributeCarrier::parseAttributeFromXML<RGBColor>(attrs, vehicleTypeID, tag, SUMO_ATTR_COLOR, abort);
    SUMOVehicleClass vClass = GNEAttributeCarrier::parseAttributeFromXML<SUMOVehicleClass>(attrs, vehicleTypeID, tag, SUMO_ATTR_VCLASS, abort);
    std::string emissionClass = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, vehicleTypeID, tag, SUMO_ATTR_EMISSIONCLASS, abort);
    SUMOVehicleShape shape = GNEAttributeCarrier::parseAttributeFromXML<SUMOVehicleShape>(attrs, vehicleTypeID, tag, SUMO_ATTR_GUISHAPE, abort);
    double width = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, vehicleTypeID, tag, SUMO_ATTR_WIDTH, abort);
    std::string filename = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, vehicleTypeID, tag, SUMO_ATTR_IMGFILE, abort);
    double impatience = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, vehicleTypeID, tag, SUMO_ATTR_IMPATIENCE, abort);
    std::string laneChangeModel = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, vehicleTypeID, tag, SUMO_ATTR_LANE_CHANGE_MODEL, abort);
    std::string carFollowModel = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, vehicleTypeID, tag, SUMO_ATTR_CAR_FOLLOW_MODEL, abort);
    int personCapacity = GNEAttributeCarrier::parseAttributeFromXML<int>(attrs, vehicleTypeID, tag, SUMO_ATTR_PERSON_CAPACITY, abort);
    int containerCapacity = GNEAttributeCarrier::parseAttributeFromXML<int>(attrs, vehicleTypeID, tag, SUMO_ATTR_CONTAINER_CAPACITY, abort);
    double boardingDuration = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, vehicleTypeID, tag, SUMO_ATTR_BOARDING_DURATION, abort);
    double loadingDuration = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, vehicleTypeID, tag, SUMO_ATTR_LOADING_DURATION, abort);
    std::string latAlignment = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, vehicleTypeID, tag, SUMO_ATTR_LATALIGNMENT, abort);
    double minGapLat = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, vehicleTypeID, tag, SUMO_ATTR_MINGAP_LAT, abort);
    double maxSpeedLat = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, vehicleTypeID, tag, SUMO_ATTR_MAXSPEED_LAT, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // check that all elements are valid
        if (myViewNet->getNet()->retrieveAdditional(SUMO_TAG_VTYPE, vehicleTypeID, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + vehicleTypeID + "'.");
        } else if(buildVehicleType(myViewNet, true, vehicleTypeID, accel, decel, sigma, tau, length, minGap, maxSpeed, speedFactor, speedDev, color, vClass, emissionClass, shape, width, 
                                   filename, impatience, laneChangeModel, carFollowModel, personCapacity, containerCapacity, boardingDuration, loadingDuration, latAlignment, minGapLat, maxSpeedLat)) {
            // save ID of last created element
            myParentElements.commitElementInsertion(vehicleTypeID);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildCalibratorFlow(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of calibrator flows
    std::string vehicleTypeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_TYPE, abort);
    std::string routeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ROUTE, abort);
    std::string vehsPerHour = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_VEHSPERHOUR, abort);
    std::string speed = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_SPEED, abort);
    RGBColor color = GNEAttributeCarrier::parseAttributeFromXML<RGBColor>(attrs, "", tag, SUMO_ATTR_COLOR, abort);
    std::string departLane = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_DEPARTLANE, abort);
    std::string departPos = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_DEPARTPOS, abort);
    std::string departSpeed = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_DEPARTSPEED, abort);
    std::string arrivalLane = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ARRIVALLANE, abort);
    std::string arrivalPos = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ARRIVALPOS, abort);
    std::string arrivalSpeed = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ARRIVALSPEED, abort);
    std::string line = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_LINE, abort);
    int personNumber = GNEAttributeCarrier::parseAttributeFromXML<int>(attrs, "", tag, SUMO_ATTR_PERSON_NUMBER, abort);
    int containerNumber = GNEAttributeCarrier::parseAttributeFromXML<int>(attrs, "", tag, SUMO_ATTR_CONTAINER_NUMBER, abort);
    bool reroute = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, "", tag, SUMO_ATTR_REROUTE, abort);
    std::string departPosLat = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_DEPARTPOS_LAT, abort);
    std::string arrivalPosLat = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ARRIVALPOS_LAT, abort);
    double begin = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_BEGIN, abort);
    double end = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_END, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // obtain route, vehicle type and calibrator parent
        GNEAdditional* calibrator = myParentElements.retrieveAdditionalParent(myViewNet, SUMO_TAG_CALIBRATOR);
        GNEAdditional* route = myViewNet->getNet()->retrieveAdditional(SUMO_TAG_ROUTE, routeID, false);
        GNEAdditional* vtype = myViewNet->getNet()->retrieveAdditional(SUMO_TAG_VTYPE, vehicleTypeID, false);
        // check that all elements are valid
        if (route == nullptr) {
            WRITE_WARNING(toString(SUMO_TAG_ROUTE) + " with ID = '" + routeID + "' cannot be created; their " + toString(SUMO_TAG_ROUTE) + " with ID = '" + routeID + "' doesn't exist");
            abort = true;
        } else if (vtype == nullptr) {
            WRITE_WARNING(toString(SUMO_TAG_VTYPE) + " with ID = '" + vehicleTypeID + "' cannot be created; their " + toString(SUMO_TAG_VTYPE) + " with ID = '" + vehicleTypeID + "' doesn't exist");
            abort = true;
        } else if ((vehsPerHour.empty()) && (speed.empty())) {
            WRITE_WARNING(toString(SUMO_TAG_VTYPE) + " with ID = '" + vehicleTypeID + "' cannot be created; At least parameters " + toString(SUMO_ATTR_VEHSPERHOUR) + " or " + toString(SUMO_ATTR_SPEED) + " has to be defined");
            abort = true;
        } else if(calibrator != NULL) {
            buildCalibratorFlow(myViewNet, true, calibrator, route, vtype, vehsPerHour, speed, color, departLane, departPos, departSpeed, arrivalLane, arrivalPos, arrivalSpeed,
                                line, personNumber, containerNumber, reroute, departPosLat, arrivalPosLat, begin, end);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildVariableSpeedSign(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of VSS
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_FILE, abort);
    std::string lanesIDs = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_LANES, abort);
    Position pos = GNEAttributeCarrier::parseAttributeFromXML<Position>(attrs, id, tag, SUMO_ATTR_POSITION, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // obtain lanes
        std::vector<GNELane*> lanes;
        if (GNEAttributeCarrier::canParse<std::vector<GNELane*> >(myViewNet->getNet(), lanesIDs, true)) {
            lanes = GNEAttributeCarrier::parse<std::vector<GNELane*> >(myViewNet->getNet(), lanesIDs);
        }
        // check that all elements are valid
        if (myViewNet->getNet()->retrieveAdditional(tag, id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + id + "'.");
        } else if ((lanes.size() > 0) && buildVariableSpeedSign(myViewNet, myUndoAdditionals, id, pos, lanes, false)) {
            // save ID of last created element
            myParentElements.commitElementInsertion(id);
        }
    }
}

void
GNEAdditionalHandler::parseAndBuildVariableSpeedSignStep(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // Load step values
    double time = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_TIME, abort);
    double speed = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_SPEED, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get Variable Speed Signal
        GNEAdditional* variableSpeedSign = myParentElements.retrieveAdditionalParent(myViewNet, SUMO_TAG_VSS);
        // check that all parameters are valid
        if (variableSpeedSign != nullptr) {
            buildVariableSpeedSignStep(myViewNet, true, variableSpeedSign, time, speed);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildRerouter(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of Rerouter
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    std::string edgesIDs = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_EDGES, abort);
    std::string file = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_FILE, abort);
    double probability = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_PROB, abort);
    bool off = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, tag, SUMO_ATTR_OFF, abort);
    double timeThreshold = attrs.getOpt<double>(SUMO_ATTR_HALTING_TIME_THRESHOLD, id.c_str(), abort, 0);
    const std::string vTypes = attrs.getOpt<std::string>(SUMO_ATTR_VTYPES, id.c_str(), abort, "");
    Position pos = GNEAttributeCarrier::parseAttributeFromXML<Position>(attrs, id, tag, SUMO_ATTR_POSITION, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // obtain edges
        std::vector<GNEEdge*> edges;
        if (GNEAttributeCarrier::canParse<std::vector<GNEEdge*> >(myViewNet->getNet(), edgesIDs, true)) {
            edges = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(myViewNet->getNet(), edgesIDs);
        }
        // check that all elements are valid
        if (myViewNet->getNet()->retrieveAdditional(tag, id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + id + "'.");
        } else if ((edgesIDs.size() > 0) && buildRerouter(myViewNet, myUndoAdditionals, id, pos, edges, probability, file, off, timeThreshold, vTypes, false)) {
            // save ID of last created element
            myParentElements.commitElementInsertion(id);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildRerouterInterval(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of Rerouter
    double begin = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_BEGIN, abort);
    double end = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_END, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // obtain rerouter
        GNEAdditional* rerouter = myParentElements.retrieveAdditionalParent(myViewNet, SUMO_TAG_REROUTER);
        // special case for load multiple intervals in the same rerouter
        if(rerouter == nullptr) {
            GNEAdditional* lastInsertedRerouterInterval = myParentElements.retrieveAdditionalParent(myViewNet, SUMO_TAG_INTERVAL);
            if(lastInsertedRerouterInterval) {
                rerouter = lastInsertedRerouterInterval->getFirstAdditionalParent();
            }
        }
        // check that rerouterInterval can be created
        if(begin >= end) {
            WRITE_WARNING(toString(tag) + " cannot be created; Attribute " + toString(SUMO_ATTR_END) + " must be greather than " + toString(SUMO_ATTR_BEGIN) + ".");
        } else if (rerouter != nullptr){
            // if rerouter interval was sucesfully created, save ID in myParentElements
            if(buildRerouterInterval(myViewNet, true, rerouter, begin, end)) {
                myParentElements.commitElementInsertion(rerouter->getAdditionalChilds().back()->getID());
            }
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildRerouterClosingLaneReroute(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of Rerouter
    std::string laneID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    std::string allow = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ALLOW, abort);
    std::string disallow = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_DISALLOW, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // obtain lane and rerouter interval
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneID, false, true);
        GNEAdditional* rerouterInterval = myParentElements.retrieveAdditionalParent(myViewNet, SUMO_TAG_INTERVAL);
        // check that all elements are valid
        if (lane == nullptr) {
            WRITE_WARNING("The lane '" + laneID + "' to use within the " + toString(tag) + " is not known.");
        } else if (rerouterInterval != nullptr) {
            buildClosingLaneReroute(myViewNet, true, rerouterInterval, lane, parseVehicleClasses(allow, disallow));
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildRerouterClosingReroute(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of Rerouter
    std::string edgeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    std::string allow = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ALLOW, abort);
    std::string disallow = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_DISALLOW, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // obtain edge and rerouter interval
        GNEEdge* edge = myViewNet->getNet()->retrieveEdge(edgeID, false);
        GNEAdditional* rerouterInterval = myParentElements.retrieveAdditionalParent(myViewNet, SUMO_TAG_INTERVAL);
        // check that all elements are valid
        if (edge == nullptr) {
            WRITE_WARNING("The edge '" + edgeID + "' to use within the " + toString(tag) + " is not known.");
        } else if (rerouterInterval != nullptr){
            buildClosingReroute(myViewNet, true, rerouterInterval, edge, parseVehicleClasses(allow, disallow));
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildRerouterDestProbReroute(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of Rerouter
    std::string edgeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    double probability = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_PROB, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // obtain edge and rerouter interval
        GNEEdge* edge = myViewNet->getNet()->retrieveEdge(edgeID, false);
        GNEAdditional* rerouterInterval = myParentElements.retrieveAdditionalParent(myViewNet, SUMO_TAG_INTERVAL);
        // check that all elements are valid
        if (edge == nullptr) {
            WRITE_WARNING("The edge '" + edgeID + "' to use within the " + toString(tag) + " is not known.");
        } else if (rerouterInterval != nullptr) {
            builDestProbReroute(myViewNet, true, rerouterInterval, edge, probability);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildRerouterParkingAreaReroute(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of Rerouter
    std::string parkingAreaID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    double probability = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_PROB, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // obtain edge and rerouter interval
        GNEAdditional* parkingArea = myViewNet->getNet()->retrieveAdditional(SUMO_TAG_PARKING_AREA, parkingAreaID, false);
        GNEAdditional* rerouterInterval = myParentElements.retrieveAdditionalParent(myViewNet, SUMO_TAG_INTERVAL);
        // check that all elements are valid
        if (parkingArea == nullptr) {
            WRITE_WARNING("The parkingArea '" + parkingAreaID + "' to use within the " + toString(tag) + " is not known.");
        } else if (rerouterInterval != nullptr) {
            builParkingAreaReroute(myViewNet, true, rerouterInterval, parkingArea, probability);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildRerouterRouteProbReroute(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of Rerouter
    std::string routeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    double probability = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_PROB, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // obtain rerouter interval
        GNEAdditional* rerouterInterval = myParentElements.retrieveAdditionalParent(myViewNet, SUMO_TAG_INTERVAL);
        // check that all elements are valid
        if (rerouterInterval != nullptr) {
            buildRouteProbReroute(myViewNet, true, rerouterInterval, routeID, probability);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildBusStop(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of bus stop
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_LANE, abort);
    std::string startPos = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_STARTPOS, abort);
    std::string endPos = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_ENDPOS, abort);
    std::string name = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_NAME, abort);
    std::vector<std::string> lines = GNEAttributeCarrier::parseAttributeFromXML<std::vector<std::string> >(attrs, id, tag, SUMO_ATTR_LINES, abort);
    bool friendlyPosition = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, tag, SUMO_ATTR_FRIENDLY_POS, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false, true);
        // check that all elements are valid
        if (myViewNet->getNet()->retrieveAdditional(tag, id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + id + "'.");
        } else if (lane == nullptr) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else if (!fixStoppinPlacePosition(startPos, endPos, lane->getParentEdge().getNBEdge()->getFinalLength(), POSITION_EPS, friendlyPosition)) {
            // Write error if position isn't valid
            WRITE_WARNING("Invalid position for " + toString(tag) + " with ID = '" + id + "'.");
        } else if(buildBusStop(myViewNet, myUndoAdditionals, id, lane, startPos, endPos, name, lines, friendlyPosition, false)){
            // save ID of last created element
            myParentElements.commitElementInsertion(id);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildContainerStop(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of container stop
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_LANE, abort);
    std::string startPos = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_STARTPOS, abort);
    std::string endPos = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_ENDPOS, abort);
    std::string name = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_NAME, abort);
    std::vector<std::string> lines = GNEAttributeCarrier::parseAttributeFromXML<std::vector<std::string> >(attrs, id, tag, SUMO_ATTR_LINES, abort);
    bool friendlyPosition = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, tag, SUMO_ATTR_FRIENDLY_POS, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false, true);
        // check that all elements are valid
        if (myViewNet->getNet()->retrieveAdditional(tag, id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + id + "'.");
        } else if (lane == nullptr) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else if (!fixStoppinPlacePosition(startPos, endPos, lane->getParentEdge().getNBEdge()->getFinalLength(), POSITION_EPS, friendlyPosition)) {
            // write error if position isn't valid
            WRITE_WARNING("Invalid position for " + toString(tag) + " with ID = '" + id + "'.");
        } else if(buildContainerStop(myViewNet, myUndoAdditionals, id, lane, startPos, endPos, name, lines, friendlyPosition, false)){
            // save ID of last created element
            myParentElements.commitElementInsertion(id);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildAccess(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of Entry
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_LANE, abort);
    std::string position = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_POSITION, abort);
    std::string length = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_LENGTH, abort);
    bool friendlyPos = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, "", tag, SUMO_ATTR_FRIENDLY_POS, abort);
    // Check if parsing of parameters was correct
    if (!abort) {
        double posDouble = GNEAttributeCarrier::parse<double>(position);
        // get lane and busStop parent
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false, true);
        GNEAdditional* busStop = myParentElements.retrieveAdditionalParent(myViewNet, SUMO_TAG_BUS_STOP);
        // check that all parameters are valid
        if (lane == nullptr) {
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " is not known.");
        } else if (busStop == nullptr) {
            WRITE_WARNING("A " + toString(tag) + " must be declared within the definition of a " + toString(SUMO_TAG_BUS_STOP) + ".");
        } else if (!checkAndFixDetectorPositionPosition(posDouble, lane->getLaneShapeLength(), friendlyPos)) {
            WRITE_WARNING("Invalid position for " + toString(tag) + ".");
        } else if(!accessCanBeCreated(busStop, lane->getParentEdge())) {
            WRITE_WARNING("Edge '" + lane->getParentEdge().getID() + "' already has an Access for busStop '" + busStop->getID() + "'");
        } else {
            buildAccess(myViewNet, myUndoAdditionals, busStop, lane, toString(posDouble), length, friendlyPos, false);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildChargingStation(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of charging station
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_LANE, abort);
    std::string startPos = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_STARTPOS, abort);
    std::string endPos = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_ENDPOS, abort);
    std::string name = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_NAME, abort);
    double chargingPower = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_CHARGINGPOWER, abort);
    double efficiency = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_EFFICIENCY, abort);
    bool chargeInTransit = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, tag, SUMO_ATTR_CHARGEINTRANSIT, abort);
    double chargeDelay = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_CHARGEDELAY, abort);
    bool friendlyPosition = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, tag, SUMO_ATTR_FRIENDLY_POS, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false, true);
        // check that all elements are valid
        if (myViewNet->getNet()->retrieveAdditional(tag, id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + id + "'.");
        } else if (lane == nullptr) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else if (!fixStoppinPlacePosition(startPos, endPos, lane->getParentEdge().getNBEdge()->getFinalLength(), POSITION_EPS, friendlyPosition)) {
            // write error if position isn't valid
            WRITE_WARNING("Invalid position for " + toString(tag) + " with ID = '" + id + "'.");
        } else if(buildChargingStation(myViewNet, myUndoAdditionals, id, lane, startPos, endPos, name, chargingPower, efficiency, chargeInTransit, chargeDelay, friendlyPosition, false)) {
            // save ID of last created element
            myParentElements.commitElementInsertion(id);
        }
    }
}


void 
GNEAdditionalHandler::parseAndBuildParkingArea(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of charging station
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_LANE, abort);
    std::string startPos = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_STARTPOS, abort);
    std::string endPos = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_ENDPOS, abort);
    std::string name = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_NAME, abort);
    bool friendlyPosition = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, tag, SUMO_ATTR_FRIENDLY_POS, abort);
    int roadSideCapacity = GNEAttributeCarrier::parseAttributeFromXML<int>(attrs, id, tag, SUMO_ATTR_ROADSIDE_CAPACITY, abort);
    double width = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_WIDTH, abort);
    std::string length = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_LENGTH, abort);
    double angle = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_ANGLE, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false, true);
        // check that all elements are valid
        if (myViewNet->getNet()->retrieveAdditional(tag, id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + id + "'.");
        } else if (lane == nullptr) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else if (!fixStoppinPlacePosition(startPos, endPos, lane->getParentEdge().getNBEdge()->getFinalLength(), POSITION_EPS, friendlyPosition)) {
            // write error if position isn't valid
            WRITE_WARNING("Invalid position for " + toString(tag) + " with ID = '" + id + "'.");
        } else if(buildParkingArea(myViewNet, myUndoAdditionals, id, lane, startPos, endPos, name, friendlyPosition, roadSideCapacity, width, length, angle, false)) {
            // save ID of last created element
            myParentElements.commitElementInsertion(id);        }
    }
}


void 
GNEAdditionalHandler::parseAndBuildParkingSpace(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of charging station
    double x = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_X, abort);
    double y = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_Y, abort);
    double z = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_Z, abort);
    double width = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_WIDTH, abort);
    double length = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_LENGTH, abort);
    double angle = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_ANGLE, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get Parking Area Parent
        GNEAdditional* parkingAreaParent = myParentElements.retrieveAdditionalParent(myViewNet, SUMO_TAG_PARKING_AREA);
        // check that all parameters are valid
        if (parkingAreaParent != nullptr) {
            buildParkingSpace(myViewNet, myUndoAdditionals, parkingAreaParent, x, y, z, width, length, angle, false);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildCalibrator(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // due there is two differents calibrators, has to be parsed in a different way
    std::string edgeID, laneId, id;
    SumoXMLTag typeOfCalibrator = tag;
    // change tag depending of XML parmeters
    if (attrs.hasAttribute(SUMO_ATTR_EDGE)) {
        typeOfCalibrator = SUMO_TAG_CALIBRATOR;
        id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", typeOfCalibrator, SUMO_ATTR_ID, abort);
        edgeID = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, typeOfCalibrator, SUMO_ATTR_EDGE, abort);
        std::string outfile = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, typeOfCalibrator, SUMO_ATTR_OUTPUT, abort);
        double position = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, typeOfCalibrator, SUMO_ATTR_POSITION, abort);
        double freq = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, typeOfCalibrator, SUMO_ATTR_FREQUENCY, abort);
        // std::string routeProbe = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_ROUTEPROBE, abort); Currently routeProbe not used
        // Continue if all parameters were sucesfully loaded
        if (!abort) {
            // get pointer and edge
            GNEEdge* edge = myViewNet->getNet()->retrieveEdge(edgeID, false);
            // check that all elements are valid
            if (myViewNet->getNet()->retrieveAdditional(typeOfCalibrator, id, false) != nullptr) {
                WRITE_WARNING("There is another " + toString(typeOfCalibrator) + " with the same ID='" + id + "'.");
            } else if (edge == nullptr) {
                WRITE_WARNING("The  edge '" + edgeID + "' to use within the " + toString(typeOfCalibrator) + " '" + id + "' is not known.");
            } else if (buildCalibrator(myViewNet, myUndoAdditionals, id, edge, position, outfile, freq)) {
                // save ID of last created element
                myParentElements.commitElementInsertion(id);
            }
        }
    } else if (attrs.hasAttribute(SUMO_ATTR_LANE)) {
        typeOfCalibrator = SUMO_TAG_LANECALIBRATOR;
        id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", typeOfCalibrator, SUMO_ATTR_ID, abort);
        laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, typeOfCalibrator, SUMO_ATTR_LANE, abort);
        std::string outfile = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, typeOfCalibrator, SUMO_ATTR_OUTPUT, abort);
        double position = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, typeOfCalibrator, SUMO_ATTR_POSITION, abort);
        double freq = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, typeOfCalibrator, SUMO_ATTR_FREQUENCY, abort);
        // std::string routeProbe = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_ROUTEPROBE, abort); Currently routeProbe not used

        // Continue if all parameters were sucesfully loaded
        if (!abort) {
            // get pointer to lane
            GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false, true);
            // check that all elements are valid
            if (myViewNet->getNet()->retrieveAdditional(typeOfCalibrator, id, false) != nullptr) {
                WRITE_WARNING("There is another " + toString(typeOfCalibrator) + " with the same ID='" + id + "'.");
            } else if (lane == nullptr) {
                WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(typeOfCalibrator) + " '" + id + "' is not known.");
            } else if (buildCalibrator(myViewNet, myUndoAdditionals, id, lane, position, outfile, freq)) {
                // save ID of last created element
                myParentElements.commitElementInsertion(id);
            }
        }
    } else {
        WRITE_WARNING("additional " + toString(tag) + " must have either a lane or an edge attribute.");
    }
}


void
GNEAdditionalHandler::parseAndBuildDetectorE1(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of E1
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_LANE, abort);
    double position = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_POSITION, abort);
    double frequency = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_FREQUENCY, abort);
    std::string file = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_FILE, abort);
    std::string vehicleTypes = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_VTYPES, abort);
    bool friendlyPos = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, tag, SUMO_ATTR_FRIENDLY_POS, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false, true);
        // check that all elements are valid
        if (myViewNet->getNet()->retrieveAdditional(tag, id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + id + "'.");
        } else if (lane == nullptr) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else if (!checkAndFixDetectorPositionPosition(position, lane->getLaneShapeLength(), friendlyPos)) {
            WRITE_WARNING("Invalid position for " + toString(tag) + " with ID = '" + id + "'.");
        } else if(buildDetectorE1(myViewNet, myUndoAdditionals, id, lane, position, frequency, file, vehicleTypes, friendlyPos, false)){
            // save ID of last created element
            myParentElements.commitElementInsertion(id);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildDetectorE2(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of E2
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_LANE, abort);
    double position = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_POSITION, abort);
    double frequency = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_FREQUENCY, abort);
    std::string file = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_FILE, abort);
    double length = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_LENGTH, abort);
    double haltingTimeThreshold = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_HALTING_TIME_THRESHOLD, abort);
    double haltingSpeedThreshold = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_HALTING_SPEED_THRESHOLD, abort);
    double jamDistThreshold = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_JAM_DIST_THRESHOLD, abort);
    bool cont = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, tag, SUMO_ATTR_CONT, abort);
    bool friendlyPos = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, tag, SUMO_ATTR_FRIENDLY_POS, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false, true);
        // check that all elements are valid
        if (myViewNet->getNet()->retrieveAdditional(tag, id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + id + "'.");
        } else if (lane == nullptr) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else if (!fixE2DetectorPositionPosition(position, length, lane->getParentEdge().getNBEdge()->getFinalLength(), friendlyPos)) {
            WRITE_WARNING("Invalid position for " + toString(tag) + " with ID = '" + id + "'.");
        } else if(buildDetectorE2(myViewNet, myUndoAdditionals, id, lane, position, length, frequency, file, cont, haltingTimeThreshold, haltingSpeedThreshold, jamDistThreshold, friendlyPos, false)) {
            // save ID of last created element
            myParentElements.commitElementInsertion(id);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildDetectorE3(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of E3
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    double frequency = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_FREQUENCY, abort);
    std::string file = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_FILE, abort);
    double haltingTimeThreshold = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_HALTING_TIME_THRESHOLD, abort);
    double haltingSpeedThreshold = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_HALTING_SPEED_THRESHOLD, abort);
    Position pos = GNEAttributeCarrier::parseAttributeFromXML<Position>(attrs, id, tag, SUMO_ATTR_POSITION, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // check that all elements are valid
        if (myViewNet->getNet()->retrieveAdditional(tag, id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + id + "'.");
        } else if (buildDetectorE3(myViewNet, myUndoAdditionals, id, pos, frequency, file, haltingTimeThreshold, haltingSpeedThreshold, false)) {
            // save ID of last created element
            myParentElements.commitElementInsertion(id);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildDetectorEntry(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of Entry
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_LANE, abort);
    double position = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_POSITION, abort);
    bool friendlyPos = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, "", tag, SUMO_ATTR_FRIENDLY_POS, abort);
    // Check if parsing of parameters was correct
    if (!abort) {
        // get lane and E3 parent
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false, true);
        GNEAdditional* E3Parent = myParentElements.retrieveAdditionalParent(myViewNet, SUMO_TAG_E3DETECTOR);
        // check that all parameters are valid
        if (lane == nullptr) {
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " is not known.");
        } else if (!checkAndFixDetectorPositionPosition(position, lane->getLaneShapeLength(), friendlyPos)) {
            WRITE_WARNING("Invalid position for " + toString(tag) + ".");
        } else {
            buildDetectorEntry(myViewNet, myUndoAdditionals, E3Parent, lane, position, friendlyPos, false);
        }
    }
}


void
GNEAdditionalHandler::parseAndBuildDetectorExit(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of Exit
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_LANE, abort);
    double position = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, "", tag, SUMO_ATTR_POSITION, abort);
    bool friendlyPos = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, "", tag, SUMO_ATTR_FRIENDLY_POS, abort);
    // Check if parsing of parameters was correct
    if (!abort) {
        // get lane and E3 parent
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false, true);
        GNEAdditional* E3Parent =myParentElements.retrieveAdditionalParent(myViewNet, SUMO_TAG_E3DETECTOR);
        // check that all parameters are valid
        if (lane == nullptr) {
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " is not known.");
        } else if (!checkAndFixDetectorPositionPosition(position, lane->getLaneShapeLength(), friendlyPos)) {
            WRITE_WARNING("Invalid position for " + toString(tag) + ".");
        } else {
            buildDetectorExit(myViewNet, myUndoAdditionals, E3Parent, lane, position, friendlyPos, false);
        }
    }
}


void 
GNEAdditionalHandler::parseAndBuildDetectorE1Instant(const SUMOSAXAttributes& attrs, const SumoXMLTag& tag) {
    bool abort = false;
    // parse attributes of E1Instant
    std::string id = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, "", tag, SUMO_ATTR_ID, abort);
    std::string laneId = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_LANE, abort);
    double position = GNEAttributeCarrier::parseAttributeFromXML<double>(attrs, id, tag, SUMO_ATTR_POSITION, abort);
    std::string file = GNEAttributeCarrier::parseAttributeFromXML<std::string>(attrs, id, tag, SUMO_ATTR_FILE, abort);
    bool friendlyPos = GNEAttributeCarrier::parseAttributeFromXML<bool>(attrs, id, tag, SUMO_ATTR_FRIENDLY_POS, abort);
    // Continue if all parameters were sucesfully loaded
    if (!abort) {
        // get pointer to lane
        GNELane* lane = myViewNet->getNet()->retrieveLane(laneId, false, true);
        // check that all elements are valid
        if (myViewNet->getNet()->retrieveAdditional(tag, id, false) != nullptr) {
            WRITE_WARNING("There is another " + toString(tag) + " with the same ID='" + id + "'.");
        } else if (lane == nullptr) {
            // Write error if lane isn't valid
            WRITE_WARNING("The lane '" + laneId + "' to use within the " + toString(tag) + " '" + id + "' is not known.");
        } else if (!checkAndFixDetectorPositionPosition(position, lane->getLaneShapeLength(), friendlyPos)) {
            WRITE_WARNING("Invalid position for " + toString(tag) + " with ID = '" + id + "'.");
        } else if (buildDetectorE1Instant(myViewNet, myUndoAdditionals, id, lane, position, file, friendlyPos, false)){
            // save ID of last created element
            myParentElements.commitElementInsertion(id);
        }
    }
}


bool
GNEAdditionalHandler::buildAdditional(GNEViewNet* viewNet, bool allowUndoRedo, SumoXMLTag tag, std::map<SumoXMLAttr, std::string> values) {
    // create additional depending of the tag
    switch (tag) {
        case SUMO_TAG_BUS_STOP: {
            // obtain specify attributes of busStop
            std::string id = values[SUMO_ATTR_ID];
            GNELane* lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            std::string startPos = values[SUMO_ATTR_STARTPOS];
            std::string endPos = values[SUMO_ATTR_ENDPOS];
            std::string name = values[SUMO_ATTR_NAME];
            std::vector<std::string> lines = GNEAttributeCarrier::parse<std::vector<std::string> >(values[SUMO_ATTR_LINES]);
            bool friendlyPos = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_FRIENDLY_POS]);
            bool blockMovement = GNEAttributeCarrier::parse<bool>(values[GNE_ATTR_BLOCK_MOVEMENT]);
            // Build busStop
            if (lane) {
                return buildBusStop(viewNet, allowUndoRedo, id, lane, startPos, endPos, name, lines, friendlyPos, blockMovement);
            } else {
                return false;
            }
        }
        case SUMO_TAG_ACCESS: {
            // obtain specify attributes of detector E2
            std::string id = values[SUMO_ATTR_ID];
            GNELane* lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            GNEAdditional* busStop = viewNet->getNet()->retrieveAdditional(SUMO_TAG_BUS_STOP, values[GNE_ATTR_PARENT], false);
            std::string pos = values[SUMO_ATTR_POSITION];
            std::string length = values[SUMO_ATTR_LENGTH];
            bool friendlyPos = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_FRIENDLY_POS]);
            bool blockMovement = GNEAttributeCarrier::parse<bool>(values[GNE_ATTR_BLOCK_MOVEMENT]);
            // Build detector E2
            if (lane && busStop) {
                if(accessCanBeCreated(busStop, lane->getParentEdge())) {
                    return buildAccess(viewNet, allowUndoRedo, busStop, lane, pos, length, friendlyPos, blockMovement);
                } else {
                    WRITE_WARNING(toString(SUMO_TAG_BUS_STOP) + "'" + busStop->getID() + "' already owns a Acces in the edge '" + lane->getParentEdge().getID() + "'");
                    return false;
                }
            } else {
                return false;
            }
        }
        case SUMO_TAG_CONTAINER_STOP: {
            // obtain specify attributes of containerStop
            std::string id = values[SUMO_ATTR_ID];
            GNELane* lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            std::string startPos = values[SUMO_ATTR_STARTPOS];
            std::string endPos = values[SUMO_ATTR_ENDPOS];
            std::string name = values[SUMO_ATTR_NAME];
            std::vector<std::string> lines = GNEAttributeCarrier::parse<std::vector<std::string> >(values[SUMO_ATTR_LINES]);
            bool friendlyPos = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_FRIENDLY_POS]);
            bool blockMovement = GNEAttributeCarrier::parse<bool>(values[GNE_ATTR_BLOCK_MOVEMENT]);
            // Build containerStop
            if (lane) {
                return buildContainerStop(viewNet, allowUndoRedo, id, lane, startPos, endPos, name, lines, friendlyPos, blockMovement);
            } else {
                return false;
            }
        }
        case SUMO_TAG_CHARGING_STATION: {
            // obtain specify attributes of chargingStation
            std::string id = values[SUMO_ATTR_ID];
            GNELane* lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            std::string startPos = values[SUMO_ATTR_STARTPOS];
            std::string endPos = values[SUMO_ATTR_ENDPOS];
            std::string name = values[SUMO_ATTR_NAME];
            double chargingPower = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_CHARGINGPOWER]);
            double efficiency = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_EFFICIENCY]);
            bool chargeInTransit = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_CHARGEINTRANSIT]);
            double chargeDelay = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_CHARGEDELAY]);
            bool friendlyPos = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_FRIENDLY_POS]);
            bool blockMovement = GNEAttributeCarrier::parse<bool>(values[GNE_ATTR_BLOCK_MOVEMENT]);
            // Build chargingStation
            if (lane) {
                return buildChargingStation(viewNet, allowUndoRedo, id, lane, startPos, endPos, name, chargingPower, efficiency, chargeInTransit, chargeDelay, friendlyPos, blockMovement);
            } else {
                return false;
            }
        }
        case SUMO_TAG_PARKING_AREA: {
            // obtain specify attributes of Parking Area
            std::string id = values[SUMO_ATTR_ID];
            GNELane* lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            std::string startPos = values[SUMO_ATTR_STARTPOS];
            std::string endPos = values[SUMO_ATTR_ENDPOS];
            std::string name = values[SUMO_ATTR_NAME];
            bool friendlyPos = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_FRIENDLY_POS]);
            int roadSideCapacity = GNEAttributeCarrier::parse<int>(values[SUMO_ATTR_ROADSIDE_CAPACITY]);
            double width = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_WIDTH]);
            std::string lenght = values[SUMO_ATTR_LENGTH];
            double angle = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_ANGLE]);
            bool blockMovement = GNEAttributeCarrier::parse<bool>(values[GNE_ATTR_BLOCK_MOVEMENT]);
            // Build Parking Area
            if (lane) {
                return buildParkingArea(viewNet, allowUndoRedo, id, lane, startPos, endPos, name, friendlyPos, roadSideCapacity, width, lenght, angle, blockMovement);
            } else {
                return false;
            }
        }
        case SUMO_TAG_PARKING_SPACE: {
            bool ok;
            // obtain specify attributes of Parking Space
            Position pos = GeomConvHelper::parseShapeReporting(values[SUMO_ATTR_POSITION], "user-supplied position", 0, ok, false)[0];
            double z = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_Z]);
            double width = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_WIDTH]);
            double lenght = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_LENGTH]);
            double angle = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_ANGLE]);
            GNEAdditional* parkingArea = viewNet->getNet()->retrieveAdditional(SUMO_TAG_PARKING_AREA, values[GNE_ATTR_PARENT], false);
            bool blockMovement = GNEAttributeCarrier::parse<bool>(values[GNE_ATTR_BLOCK_MOVEMENT]);
            // Build Parking
            return buildParkingSpace(viewNet, allowUndoRedo, parkingArea, pos.x(), pos.y(), z, width, lenght, angle, blockMovement);
        }
        case SUMO_TAG_E1DETECTOR: {
            // obtain specify attributes of detector E1
            std::string id = values[SUMO_ATTR_ID];
            GNELane* lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            double pos = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_POSITION]);
            double freq = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_FREQUENCY]);
            std::string filename = values[SUMO_ATTR_FILE];
            std::string vehicleTypes = values[SUMO_ATTR_VTYPES];
            bool friendlyPos = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_FRIENDLY_POS]);
            bool blockMovement = GNEAttributeCarrier::parse<bool>(values[GNE_ATTR_BLOCK_MOVEMENT]);
            // Build detector E1
            if (lane) {
                return buildDetectorE1(viewNet, allowUndoRedo, id, lane, pos, freq, filename, vehicleTypes, friendlyPos, blockMovement);
            } else {
                return false;
            }
        }
        case SUMO_TAG_E2DETECTOR: {
            // obtain specify attributes of detector E2
            std::string id = values[SUMO_ATTR_ID];
            GNELane* lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            double pos = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_POSITION]);
            double freq = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_FREQUENCY]);
            double length = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_LENGTH]);
            std::string filename = values[SUMO_ATTR_FILE];
            bool cont = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_CONT]);
            double timeThreshold = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_HALTING_TIME_THRESHOLD]);
            double speedThreshold = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_HALTING_SPEED_THRESHOLD]);
            double jamThreshold = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_JAM_DIST_THRESHOLD]);
            bool friendlyPos = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_FRIENDLY_POS]);
            bool blockMovement = GNEAttributeCarrier::parse<bool>(values[GNE_ATTR_BLOCK_MOVEMENT]);
            // Build detector E2
            if (lane) {
                return buildDetectorE2(viewNet, allowUndoRedo, id, lane, pos, length, freq, filename, cont, timeThreshold, speedThreshold, jamThreshold, friendlyPos, blockMovement);
            } else {
                return false;
            }
        }
        case SUMO_TAG_E3DETECTOR: {
            // obtain specify attributes of detector E3
            bool ok;
            std::string id = values[SUMO_ATTR_ID];
            PositionVector pos = GeomConvHelper::parseShapeReporting(values[SUMO_ATTR_POSITION], "user-supplied position", 0, ok, false);
            double freq = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_FREQUENCY]);
            std::string filename = values[SUMO_ATTR_FILE];
            double timeThreshold = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_HALTING_TIME_THRESHOLD]);
            double speedThreshold = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_HALTING_SPEED_THRESHOLD]);
            bool blockMovement = GNEAttributeCarrier::parse<bool>(values[GNE_ATTR_BLOCK_MOVEMENT]);
            // Build detector E3
            if (pos.size() == 1) {
                return buildDetectorE3(viewNet, allowUndoRedo, id, pos[0], freq, filename, timeThreshold, speedThreshold, blockMovement);
            } else {
                return false;
            }
        }
        case SUMO_TAG_DET_ENTRY: {
            // obtain specify attributes of detector Entry
            GNELane* lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            double pos = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_POSITION]);
            bool friendlyPos = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_FRIENDLY_POS]);
            GNEAdditional* E3 = viewNet->getNet()->retrieveAdditional(SUMO_TAG_E3DETECTOR, values[GNE_ATTR_PARENT], false);
            bool blockMovement = GNEAttributeCarrier::parse<bool>(values[GNE_ATTR_BLOCK_MOVEMENT]);
            // Build detector Entry
            if (lane && E3) {
                return buildDetectorEntry(viewNet, allowUndoRedo, E3, lane, pos, friendlyPos, blockMovement);
            } else {
                return false;
            }
        }
        case SUMO_TAG_DET_EXIT: {
            // obtain specify attributes of Detector Exit
            GNELane* lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            double pos = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_POSITION]);
            bool friendlyPos = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_FRIENDLY_POS]);
            GNEAdditional* E3 = viewNet->getNet()->retrieveAdditional(SUMO_TAG_E3DETECTOR, values[GNE_ATTR_PARENT], false);
            bool blockMovement = GNEAttributeCarrier::parse<bool>(values[GNE_ATTR_BLOCK_MOVEMENT]);
            // Build detector Exit
            if (lane && E3) {
                return buildDetectorExit(viewNet, allowUndoRedo, E3, lane, pos, friendlyPos, blockMovement);
            } else {
                return false;
            }
        }
        case SUMO_TAG_INSTANT_INDUCTION_LOOP: {
            // obtain specify attributes of detector E1Instant
            std::string id = values[SUMO_ATTR_ID];
            GNELane* lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            double pos = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_POSITION]);
            std::string filename = values[SUMO_ATTR_FILE];
            bool friendlyPos = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_FRIENDLY_POS]);
            bool blockMovement = GNEAttributeCarrier::parse<bool>(values[GNE_ATTR_BLOCK_MOVEMENT]);
            // Build detector E1Instant
            if (lane) {
                return buildDetectorE1Instant(viewNet, allowUndoRedo, id, lane, pos, filename, friendlyPos, blockMovement);
            } else {
                return false;
            }
        }
        case SUMO_TAG_VSS: {
            // obtain specify attributes of variable speed signal
            std::string id = values[SUMO_ATTR_ID];
            bool ok;
            PositionVector pos = GeomConvHelper::parseShapeReporting(values[SUMO_ATTR_POSITION], "user-supplied position", 0, ok, false);
            std::vector<GNELane*> lanes = GNEAttributeCarrier::parse<std::vector<GNELane*> >(viewNet->getNet(), values[SUMO_ATTR_LANES]);
            // get rest of parameters
            bool blockMovement = GNEAttributeCarrier::parse<bool>(values[GNE_ATTR_BLOCK_MOVEMENT]);
            // build VSS
            if (pos.size() == 1) {
                return buildVariableSpeedSign(viewNet, allowUndoRedo, id, pos[0], lanes, blockMovement);
            } else {
                return false;
            }
        }
        case SUMO_TAG_CALIBRATOR: {
            // obtain specify attributes of calibrator
            std::string id = values[SUMO_ATTR_ID];
            GNEEdge* edge = viewNet->getNet()->retrieveEdge(values[SUMO_ATTR_EDGE], false);
            // get rest of parameters
            double pos = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_POSITION]);
            std::string outfile = values[SUMO_ATTR_OUTPUT];
            double freq = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_FREQUENCY]);
            // Build calibrator edge
            if (edge) {
                return buildCalibrator(viewNet, allowUndoRedo, id, edge, pos, outfile, freq);
            } else {
                return false;
            }
        }
        case SUMO_TAG_LANECALIBRATOR: {
            // obtain specify attributes of calibrator
            std::string id = values[SUMO_ATTR_ID];
            GNELane* lane = viewNet->getNet()->retrieveLane(values[SUMO_ATTR_LANE], false);
            // get rest of parameters
            double pos = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_POSITION]);
            std::string outfile = values[SUMO_ATTR_OUTPUT];
            double freq = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_FREQUENCY]);
            // Build calibrator lane
            if (lane) {
                return buildCalibrator(viewNet, allowUndoRedo, id, lane, pos, outfile, freq);
            } else {
                return false;
            }
        }
        case SUMO_TAG_REROUTER: {
            // obtain specify attributes of rerouter
            std::string id = values[SUMO_ATTR_ID];
            bool ok;
            PositionVector pos = GeomConvHelper::parseShapeReporting(values[SUMO_ATTR_POSITION], "user-supplied position", 0, ok, false);
            std::vector<GNEEdge*> edges = GNEAttributeCarrier::parse<std::vector<GNEEdge*> >(viewNet->getNet(), values[SUMO_ATTR_EDGES]);
            // Get rest of parameters
            bool off = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_OFF]);
            double prob = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_PROB]);
            double timeThreshold = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_HALTING_TIME_THRESHOLD]);
            const std::string vTypes = values[SUMO_ATTR_VTYPES];
            bool blockMovement = GNEAttributeCarrier::parse<bool>(values[GNE_ATTR_BLOCK_MOVEMENT]);
            std::string file = values[SUMO_ATTR_FILE];
            // Build rerouter
            if (pos.size() == 1) {
                return buildRerouter(viewNet, allowUndoRedo, id, pos[0], edges, prob, file, off, timeThreshold, vTypes, blockMovement);
            } else {
                return false;
            }
        }
        case SUMO_TAG_ROUTEPROBE: {
            // obtain specify attributes of RouteProbe
            std::string id = values[SUMO_ATTR_ID];
            GNEEdge* edge = viewNet->getNet()->retrieveEdge(values[SUMO_ATTR_EDGE], false);
            std::string freq = values[SUMO_ATTR_FREQUENCY];
            std::string filename = values[SUMO_ATTR_FILE];
            double begin = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_BEGIN]);
            // Build RouteProbe
            if (edge) {
                return buildRouteProbe(viewNet, allowUndoRedo, id, edge, freq, filename, begin);
            } else {
                return false;
            }
        }
        case SUMO_TAG_VAPORIZER: {
            // obtain specify attributes of vaporizer
            GNEEdge* edge = viewNet->getNet()->retrieveEdge(values[SUMO_ATTR_EDGE], false);
            double startTime = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_BEGIN]);
            double end = GNEAttributeCarrier::parse<double>(values[SUMO_ATTR_END]);
            // Build Vaporizer
            if (edge) {
                if(viewNet->getNet()->retrieveAdditional(tag, edge->getID(), false) == nullptr) { 
                    return buildVaporizer(viewNet, allowUndoRedo, edge, startTime, end);
                } else {
                    WRITE_WARNING("There is already a " + toString(tag) + " in the edge '" + edge->getID() + "'.");
                }
            } else {
                return false;
            }
        }
        default:
            return false;
    }
}


bool
GNEAdditionalHandler::buildBusStop(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNELane* lane, const std::string& startPos, const std::string& endPos, const std::string& name, const std::vector<std::string>& lines, bool friendlyPosition, bool blockMovement) {
    if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_BUS_STOP, id, false) == nullptr) {
        GNEBusStop* busStop = new GNEBusStop(id, lane, viewNet, startPos, endPos, name, lines, friendlyPosition, blockMovement);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_BUS_STOP));
            viewNet->getUndoList()->add(new GNEChange_Additional(busStop, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertAdditional(busStop);
            lane->addAdditionalChild(busStop);
            busStop->incRef("buildBusStop");
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_BUS_STOP) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


bool
GNEAdditionalHandler::buildAccess(GNEViewNet* viewNet, bool allowUndoRedo, GNEAdditional *busStop, GNELane* lane, const std::string& pos, const std::string& length, bool friendlyPos, bool blockMovement) {
    // Check if busStop parent and lane is correct
    if (lane == nullptr) {
        throw ProcessError("Could not build " + toString(SUMO_TAG_ACCESS) + " in netedit; " +  toString(SUMO_TAG_LANE) + " doesn't exist.");
    } else if (busStop == nullptr) {
        throw ProcessError("Could not build " + toString(SUMO_TAG_ACCESS) + " in netedit; " +  toString(SUMO_TAG_BUS_STOP) + " parent doesn't exist.");
    } else if (!accessCanBeCreated(busStop, lane->getParentEdge())) {
        throw ProcessError("Could not build " + toString(SUMO_TAG_ACCESS) + " in netedit; " +  toString(SUMO_TAG_BUS_STOP) + " parent already owns a Acces in the edge '" + lane->getParentEdge().getID() + "'");
    } else {
        GNEAccess* access = new GNEAccess(busStop, lane, viewNet, pos, length, friendlyPos, blockMovement);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_ACCESS));
            viewNet->getUndoList()->add(new GNEChange_Additional(access, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertAdditional(access);
            lane->addAdditionalChild(access);
            busStop->addAdditionalChild(access);
            access->incRef("buildAccess");
        }
        return true;
    }
}


bool
GNEAdditionalHandler::buildContainerStop(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNELane* lane, const std::string& startPos, const std::string& endPos, const std::string& name, const std::vector<std::string>& lines, bool friendlyPosition, bool blockMovement) {
    if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, id, false) == nullptr) {
        GNEContainerStop* containerStop = new GNEContainerStop(id, lane, viewNet, startPos, endPos, name, lines, friendlyPosition, blockMovement);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_CONTAINER_STOP));
            viewNet->getUndoList()->add(new GNEChange_Additional(containerStop, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertAdditional(containerStop);
            lane->addAdditionalChild(containerStop);
            containerStop->incRef("buildContainerStop");
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_CONTAINER_STOP) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


bool
GNEAdditionalHandler::buildChargingStation(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNELane* lane, const std::string& startPos, const std::string& endPos, const std::string& name, 
                                           double chargingPower, double efficiency, bool chargeInTransit, double chargeDelay, bool friendlyPosition, bool blockMovement) {
    if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_CHARGING_STATION, id, false) == nullptr) {
        GNEChargingStation* chargingStation = new GNEChargingStation(id, lane, viewNet, startPos, endPos, name, chargingPower, efficiency, chargeInTransit, chargeDelay, friendlyPosition, blockMovement);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_CHARGING_STATION));
            viewNet->getUndoList()->add(new GNEChange_Additional(chargingStation, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertAdditional(chargingStation);
            lane->addAdditionalChild(chargingStation);
            chargingStation->incRef("buildChargingStation");
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_CHARGING_STATION) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


bool 
GNEAdditionalHandler::buildParkingArea(GNEViewNet* viewNet, bool allowUndoRedo, const std::string &id, GNELane* lane, const std::string& startPos, const std::string &endPos, const std::string& name, 
                                       bool friendlyPosition, int roadSideCapacity, double width, const std::string &length, double angle, bool blockMovement) {
    if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_PARKING_AREA, id, false) == nullptr) {
        GNEParkingArea* ParkingArea = new GNEParkingArea(id, lane, viewNet, startPos, endPos, name, friendlyPosition, roadSideCapacity, width, length, angle, blockMovement);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_PARKING_AREA));
            viewNet->getUndoList()->add(new GNEChange_Additional(ParkingArea, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertAdditional(ParkingArea);
            lane->addAdditionalChild(ParkingArea);
            ParkingArea->incRef("buildParkingArea");
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_PARKING_AREA) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


bool 
GNEAdditionalHandler::buildParkingSpace(GNEViewNet* viewNet, bool allowUndoRedo, GNEAdditional *parkingAreaParent, double x, double y, double z, double width, double length, double angle, bool blockMovement) {
    GNEParkingSpace* ParkingSpace = new GNEParkingSpace(viewNet, parkingAreaParent, x, y, z, width, length, angle, blockMovement);
    if (allowUndoRedo) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_PARKING_SPACE));
        viewNet->getUndoList()->add(new GNEChange_Additional(ParkingSpace, true), true);
        viewNet->getUndoList()->p_end();
    } else {
        viewNet->getNet()->insertAdditional(ParkingSpace);
        parkingAreaParent->addAdditionalChild(ParkingSpace);
        ParkingSpace->incRef("buildParkingSpace");
    }
    return true;
}


bool
GNEAdditionalHandler::buildDetectorE1(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNELane* lane, double pos, double freq, const std::string& filename, const std::string& vehicleTypes, bool friendlyPos, bool blockMovement) {
    if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_E1DETECTOR, id, false) == nullptr) {
        GNEDetectorE1* detectorE1 = new GNEDetectorE1(id, lane, viewNet, pos, freq, filename, vehicleTypes, friendlyPos, blockMovement);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_E1DETECTOR));
            viewNet->getUndoList()->add(new GNEChange_Additional(detectorE1, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertAdditional(detectorE1);
            lane->addAdditionalChild(detectorE1);
            detectorE1->incRef("buildDetectorE1");
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_E1DETECTOR) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


bool
GNEAdditionalHandler::buildDetectorE2(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNELane* lane, double pos, double length, double freq, const std::string& filename,
                                      bool cont, const double timeThreshold, double speedThreshold, double jamThreshold, bool friendlyPos, bool blockMovement) {
    if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_E2DETECTOR, id, false) == nullptr) {
        GNEDetectorE2* detectorE2 = new GNEDetectorE2(id, lane, viewNet, pos, length, freq, filename, cont, timeThreshold, speedThreshold, jamThreshold, friendlyPos, blockMovement);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_E2DETECTOR));
            viewNet->getUndoList()->add(new GNEChange_Additional(detectorE2, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertAdditional(detectorE2);
            lane->addAdditionalChild(detectorE2);
            detectorE2->incRef("buildDetectorE2");
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_E2DETECTOR) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


bool
GNEAdditionalHandler::buildDetectorE3(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, Position pos, double freq, const std::string& filename, const double timeThreshold, double speedThreshold, bool blockMovement) {
    if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_E3DETECTOR, id, false) == nullptr) {
        GNEDetectorE3* detectorE3 = new GNEDetectorE3(id, viewNet, pos, freq, filename, timeThreshold, speedThreshold, blockMovement);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_E3DETECTOR));
            viewNet->getUndoList()->add(new GNEChange_Additional(detectorE3, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertAdditional(detectorE3);
            detectorE3->incRef("buildDetectorE3");
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_E3DETECTOR) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


bool
GNEAdditionalHandler::buildDetectorEntry(GNEViewNet* viewNet, bool allowUndoRedo, GNEAdditional* E3Parent, GNELane* lane, double pos, bool friendlyPos, bool blockMovement) {
    // Check if Detector E3 parent and lane is correct
    if (lane == nullptr) {
        throw ProcessError("Could not build " + toString(SUMO_TAG_DET_ENTRY) + " in netedit; " +  toString(SUMO_TAG_LANE) + " doesn't exist.");
    } else if (E3Parent == nullptr) {
        throw ProcessError("Could not build " + toString(SUMO_TAG_DET_ENTRY) + " in netedit; " +  toString(SUMO_TAG_E3DETECTOR) + " parent doesn't exist.");
    } else {
        GNEDetectorEntry* entry = new GNEDetectorEntry(viewNet, E3Parent, lane, pos, friendlyPos, blockMovement);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_DET_ENTRY));
            viewNet->getUndoList()->add(new GNEChange_Additional(entry, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertAdditional(entry);
            lane->addAdditionalChild(entry);
            E3Parent->addAdditionalChild(entry);
            entry->incRef("buildDetectorEntry");
        }
        return true;
    }
}


bool
GNEAdditionalHandler::buildDetectorExit(GNEViewNet* viewNet, bool allowUndoRedo, GNEAdditional* E3Parent, GNELane* lane, double pos, bool friendlyPos, bool blockMovement) {
    // Check if Detector E3 parent and lane is correct
    if (lane == nullptr) {
        throw ProcessError("Could not build " + toString(SUMO_TAG_DET_ENTRY) + " in netedit; " +  toString(SUMO_TAG_LANE) + " doesn't exist.");
    } else if (E3Parent == nullptr) {
        throw ProcessError("Could not build " + toString(SUMO_TAG_DET_ENTRY) + " in netedit; " +  toString(SUMO_TAG_E3DETECTOR) + " parent doesn't exist.");
    } else {
        GNEDetectorExit* exit = new GNEDetectorExit(viewNet, E3Parent, lane, pos, friendlyPos, blockMovement);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_DET_EXIT));
            viewNet->getUndoList()->add(new GNEChange_Additional(exit, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertAdditional(exit);
            lane->addAdditionalChild(exit);
            E3Parent->addAdditionalChild(exit);
            exit->incRef("buildDetectorExit");
        }
        return true;
    }
}


bool
GNEAdditionalHandler::buildDetectorE1Instant(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNELane* lane, double pos, const std::string& filename, bool friendlyPos, bool blockMovement) {
    if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_INSTANT_INDUCTION_LOOP, id, false) == nullptr) {
        GNEDetectorE1Instant* detectorE1Instant = new GNEDetectorE1Instant(id, lane, viewNet, pos, filename, friendlyPos, blockMovement);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_INSTANT_INDUCTION_LOOP));
            viewNet->getUndoList()->add(new GNEChange_Additional(detectorE1Instant, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertAdditional(detectorE1Instant);
            lane->addAdditionalChild(detectorE1Instant);
            detectorE1Instant->incRef("buildDetectorE1Instant");
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_INSTANT_INDUCTION_LOOP) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


bool
GNEAdditionalHandler::buildCalibrator(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNELane* lane, double pos, const std::string& outfile, const double freq) {
    if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_CALIBRATOR, id, false) == nullptr) {
        GNECalibrator* calibrator = new GNECalibrator(id, viewNet, lane, pos, freq, outfile);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_CALIBRATOR));
            viewNet->getUndoList()->add(new GNEChange_Additional(calibrator, true), true);
            viewNet->getUndoList()->p_end();
            // center after creation
            viewNet->centerTo(calibrator->getGlID(), false);
        } else {
            viewNet->getNet()->insertAdditional(calibrator);
            lane->addAdditionalChild(calibrator);
            calibrator->incRef("buildCalibrator");
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_CALIBRATOR) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


bool
GNEAdditionalHandler::buildCalibrator(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNEEdge* edge, double pos, const std::string& outfile, const double freq) {
    if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_CALIBRATOR, id, false) == nullptr) {
        GNECalibrator* calibrator = new GNECalibrator(id, viewNet, edge, pos, freq, outfile);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_CALIBRATOR));
            viewNet->getUndoList()->add(new GNEChange_Additional(calibrator, true), true);
            viewNet->getUndoList()->p_end();
            // center after creation
            viewNet->centerTo(calibrator->getGlID(), false);
        } else {
            viewNet->getNet()->insertAdditional(calibrator);
            edge->addAdditionalChild(calibrator);
            calibrator->incRef("buildCalibrator");
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_CALIBRATOR) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


bool
GNEAdditionalHandler::buildCalibratorRoute(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& routeID, const std::vector<GNEEdge*>& edges, const RGBColor& color) {
    if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_ROUTE, routeID, false) == nullptr) {
        // create route and add it to calibrator parent
        GNECalibratorRoute* route = new GNECalibratorRoute(viewNet, routeID, edges, color);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(route->getTag()));
            viewNet->getUndoList()->add(new GNEChange_Additional(route, true), true);
            viewNet->getUndoList()->p_end();
            return true;
        } else {
            viewNet->getNet()->insertAdditional(route);
            route->incRef("buildCalibratorRoute");
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_ROUTE) + " with ID '" + routeID + "' in netedit; probably declared twice.");
    }

}


bool
GNEAdditionalHandler::buildVehicleType(GNEViewNet* viewNet, bool allowUndoRedo, std::string vehicleTypeID,
        double accel, double decel, double sigma, double tau, double length, double minGap, double maxSpeed,
        double speedFactor, double speedDev, const RGBColor& color, SUMOVehicleClass vClass, const std::string& emissionClass,
        SUMOVehicleShape shape, double width, const std::string& filename, double impatience, const std::string& laneChangeModel,
        const std::string& carFollowModel, int personCapacity, int containerCapacity, double boardingDuration,
        double loadingDuration, const std::string& latAlignment, double minGapLat, double maxSpeedLat) {
    if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_VTYPE, vehicleTypeID, false) == nullptr) {
        // create vehicle type and add it to calibrator parent
        GNECalibratorVehicleType* vType = new GNECalibratorVehicleType(viewNet, vehicleTypeID, accel, decel, sigma, tau, length, minGap, maxSpeed,
                speedFactor, speedDev, color, vClass, emissionClass, shape, width, filename, impatience,
                laneChangeModel, carFollowModel, personCapacity, containerCapacity, boardingDuration,
                loadingDuration, latAlignment, minGapLat, maxSpeedLat);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(vType->getTag()));
            viewNet->getUndoList()->add(new GNEChange_Additional(vType, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertAdditional(vType);
            vType->incRef("buildCalibratorVehicleType");
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_VTYPE) + " with ID '" + vehicleTypeID + "' in netedit; probably declared twice.");
    }
}


bool
GNEAdditionalHandler::buildCalibratorFlow(GNEViewNet* viewNet, bool allowUndoRedo, GNEAdditional* calibratorParent, GNEAdditional* route, GNEAdditional* vtype, 
        const std::string &vehsPerHour, const std::string &speed, const RGBColor& color, const std::string& departLane, const std::string& departPos, 
        const std::string& departSpeed, const std::string& arrivalLane, const std::string& arrivalPos, const std::string& arrivalSpeed, const std::string& line, 
        int personNumber, int containerNumber, bool reroute, const std::string& departPosLat, const std::string& arrivalPosLat, double begin, double end) {

    // create Flow and add it to calibrator parent
    GNECalibratorFlow* flow = new GNECalibratorFlow(calibratorParent, vtype, route, vehsPerHour, speed, color, departLane, departPos, departSpeed,
            arrivalLane, arrivalPos, arrivalSpeed, line, personNumber, containerNumber, reroute,
            departPosLat, arrivalPosLat, begin, end);
    if (allowUndoRedo) {
        viewNet->getUndoList()->p_begin("add " + toString(flow->getTag()));
        viewNet->getUndoList()->add(new GNEChange_Additional(flow, true), true);
        viewNet->getUndoList()->p_end();
    } else {
        calibratorParent->addAdditionalChild(flow);
        flow->incRef("buildCalibratorFlow");
    }
    return true;
}


bool
GNEAdditionalHandler::buildRerouter(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, Position pos, const std::vector<GNEEdge*>& edges, double prob, const std::string& file, bool off, double timeThreshold, const std::string& vTypes, bool blockMovement) {
    if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_REROUTER, id, false) == nullptr) {
        GNERerouter* rerouter = new GNERerouter(id, viewNet, pos, edges, file, prob, off, timeThreshold, vTypes, blockMovement);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_REROUTER));
            viewNet->getUndoList()->add(new GNEChange_Additional(rerouter, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertAdditional(rerouter);
            // add this rerouter as parent of all edges
            for (auto i : edges) {
                i->addAdditionalParent(rerouter);
            }
            rerouter->incRef("buildRerouter");
        }
        // parse rerouter childs
        if(!file.empty()) {
            // we assume that rerouter values files is placed in the same folder as the additional file
            std::string currentAdditionalFilename = OptionsCont::getOptions().getString("sumo-additionals-file");
            // clear filename
            while(!currentAdditionalFilename.empty() && !(currentAdditionalFilename.back() == '\\' || currentAdditionalFilename.back() == '/')) {
                currentAdditionalFilename.pop_back();
            }
            // Create additional handler for parse rerouter values
            GNEAdditionalHandler rerouterValuesHandler(currentAdditionalFilename + file, viewNet, allowUndoRedo, rerouter);
            // Run parser
            if (!XMLSubSys::runParser(rerouterValuesHandler, currentAdditionalFilename + file, false)) {
                WRITE_MESSAGE("Loading of " + file + " failed.");
            }
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_REROUTER) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


bool
GNEAdditionalHandler::buildRerouterInterval(GNEViewNet* viewNet, bool allowUndoRedo, GNEAdditional* rerouterParent, double begin, double end) {
    // check if new interval will produce a overlapping
    if (checkOverlappingRerouterIntervals(rerouterParent, begin, end)) {
        // create rerouter interval and add it into rerouter parent
        GNERerouterInterval* rerouterInterval = new GNERerouterInterval(rerouterParent, begin, end);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(rerouterInterval->getTag()));
            viewNet->getUndoList()->add(new GNEChange_Additional(rerouterInterval, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            rerouterParent->addAdditionalChild(rerouterInterval);
            rerouterInterval->incRef("buildRerouterInterval");
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_INTERVAL) + " with begin '" + toString(begin) + "' and '" + toString(end) + "' in '" + rerouterParent->getID() + "' due overlapping.");
    }
}


bool
GNEAdditionalHandler::buildClosingLaneReroute(GNEViewNet* viewNet, bool allowUndoRedo, GNEAdditional* rerouterIntervalParent, GNELane* closedLane, SVCPermissions permissions) {
    // create closing lane reorute
    GNEClosingLaneReroute* closingLaneReroute = new GNEClosingLaneReroute(rerouterIntervalParent, closedLane, permissions);
    // add it to interval parent depending of allowUndoRedo
    if (allowUndoRedo) {
        viewNet->getUndoList()->p_begin("add " + toString(closingLaneReroute->getTag()));
        viewNet->getUndoList()->add(new GNEChange_Additional(closingLaneReroute, true), true);
        viewNet->getUndoList()->p_end();
    } else {
        rerouterIntervalParent->addAdditionalChild(closingLaneReroute);
        closingLaneReroute->incRef("buildClosingLaneReroute");
    }
    return true;
}


bool
GNEAdditionalHandler::buildClosingReroute(GNEViewNet* viewNet, bool allowUndoRedo, GNEAdditional* rerouterIntervalParent, GNEEdge* closedEdge, SVCPermissions permissions) {
    // create closing reroute
    GNEClosingReroute* closingReroute = new GNEClosingReroute(rerouterIntervalParent, closedEdge, permissions);
    // add it to interval parent depending of allowUndoRedo
    if (allowUndoRedo) {
        viewNet->getUndoList()->p_begin("add " + toString(closingReroute->getTag()));
        viewNet->getUndoList()->add(new GNEChange_Additional(closingReroute, true), true);
        viewNet->getUndoList()->p_end();
    } else {
        rerouterIntervalParent->addAdditionalChild(closingReroute);
        closingReroute->incRef("buildClosingReroute");
    }
    return true;
}


bool
GNEAdditionalHandler::builDestProbReroute(GNEViewNet* viewNet, bool allowUndoRedo, GNEAdditional* rerouterIntervalParent, GNEEdge* newEdgeDestination, double probability) {
    // create dest probability reroute
    GNEDestProbReroute* destProbReroute = new GNEDestProbReroute(rerouterIntervalParent, newEdgeDestination, probability);
    // add it to interval parent depending of allowUndoRedo
    if (allowUndoRedo) {
        viewNet->getUndoList()->p_begin("add " + toString(destProbReroute->getTag()));
        viewNet->getUndoList()->add(new GNEChange_Additional(destProbReroute, true), true);
        viewNet->getUndoList()->p_end();
    } else {
        rerouterIntervalParent->addAdditionalChild(destProbReroute);
        destProbReroute->incRef("builDestProbReroute");
    }
    return true;
}


bool
GNEAdditionalHandler::builParkingAreaReroute(GNEViewNet* viewNet, bool allowUndoRedo, GNEAdditional* rerouterIntervalParent, GNEAdditional* newParkingArea, double probability) {
    // create dest probability reroute
    GNEParkingAreaReroute* parkingAreaReroute = new GNEParkingAreaReroute(rerouterIntervalParent, newParkingArea, probability);
    // add it to interval parent depending of allowUndoRedo
    if (allowUndoRedo) {
        viewNet->getUndoList()->p_begin("add " + toString(parkingAreaReroute->getTag()));
        viewNet->getUndoList()->add(new GNEChange_Additional(parkingAreaReroute, true), true);
        viewNet->getUndoList()->p_end();
    } else {
        rerouterIntervalParent->addAdditionalChild(parkingAreaReroute);
        parkingAreaReroute->incRef("builParkingAreaReroute");
    }
    return true;
}


bool
GNEAdditionalHandler::buildRouteProbReroute(GNEViewNet* viewNet, bool allowUndoRedo, GNEAdditional* rerouterIntervalParent, const std::string& newRouteId, double probability) {
    // create rout prob rereoute
    GNERouteProbReroute* routeProbReroute = new GNERouteProbReroute(rerouterIntervalParent, newRouteId, probability);
    // add it to interval parent depending of allowUndoRedo
    if (allowUndoRedo) {
        viewNet->getUndoList()->p_begin("add " + toString(routeProbReroute->getTag()));
        viewNet->getUndoList()->add(new GNEChange_Additional(routeProbReroute, true), true);
        viewNet->getUndoList()->p_end();
    } else {
        rerouterIntervalParent->addAdditionalChild(routeProbReroute);
        routeProbReroute->incRef("buildRouteProbReroute");
    }
    return true;
}


bool
GNEAdditionalHandler::buildRouteProbe(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, GNEEdge* edge, const std::string &freq, const std::string& file, double begin) {
    if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_ROUTEPROBE, id, false) == nullptr) {
        GNERouteProbe* routeProbe = new GNERouteProbe(id, viewNet, edge, freq, file, begin);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_ROUTEPROBE));
            viewNet->getUndoList()->add(new GNEChange_Additional(routeProbe, true), true);
            viewNet->getUndoList()->p_end();
            // center after creation
            viewNet->centerTo(routeProbe->getGlID(), false);
        } else {
            viewNet->getNet()->insertAdditional(routeProbe);
            edge->addAdditionalChild(routeProbe);
            routeProbe->incRef("buildRouteProbe");
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_ROUTEPROBE) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


bool
GNEAdditionalHandler::buildVariableSpeedSign(GNEViewNet* viewNet, bool allowUndoRedo, const std::string& id, Position pos, const std::vector<GNELane*>& lanes, bool blockMovement) {
    if (viewNet->getNet()->retrieveAdditional(SUMO_TAG_VSS, id, false) == nullptr) {
        GNEVariableSpeedSign* variableSpeedSign = new GNEVariableSpeedSign(id, viewNet, pos, lanes, blockMovement);
        if (allowUndoRedo) {
            viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_VSS));
            viewNet->getUndoList()->add(new GNEChange_Additional(variableSpeedSign, true), true);
            viewNet->getUndoList()->p_end();
        } else {
            viewNet->getNet()->insertAdditional(variableSpeedSign);
            // add this VSS as parent of all edges
            for (auto i : lanes) {
                i->addAdditionalParent(variableSpeedSign);
            }
            variableSpeedSign->incRef("buildVariableSpeedSign");
        }
        return true;
    } else {
        throw ProcessError("Could not build " + toString(SUMO_TAG_VSS) + " with ID '" + id + "' in netedit; probably declared twice.");
    }
}


bool
GNEAdditionalHandler::buildVariableSpeedSignStep(GNEViewNet* viewNet, bool allowUndoRedo, GNEAdditional* VSSParent, double time, double speed) {
    // create Variable Speed Sign
    GNEVariableSpeedSignStep* variableSpeedSignStep = new GNEVariableSpeedSignStep(VSSParent, time, speed);
    // add it depending of allow undoRedo
    if (allowUndoRedo) {
        viewNet->getUndoList()->p_begin("add " + toString(variableSpeedSignStep->getTag()));
        viewNet->getUndoList()->add(new GNEChange_Additional(variableSpeedSignStep, true), true);
        viewNet->getUndoList()->p_end();
    } else {
        VSSParent->addAdditionalChild(variableSpeedSignStep);
        variableSpeedSignStep->incRef("buildVariableSpeedSignStep");
    }
    return true;
}


bool
GNEAdditionalHandler::buildVaporizer(GNEViewNet* viewNet, bool allowUndoRedo, GNEEdge* edge, double startTime, double end) {
    GNEVaporizer* vaporizer = new GNEVaporizer(viewNet, edge, startTime, end);
    if (allowUndoRedo) {
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_VAPORIZER));
        viewNet->getUndoList()->add(new GNEChange_Additional(vaporizer, true), true);
        viewNet->getUndoList()->p_end();
        // center after creation
        viewNet->centerTo(vaporizer->getGlID(), false);
    } else {
        viewNet->getNet()->insertAdditional(vaporizer);
        edge->addAdditionalChild(vaporizer);
        vaporizer->incRef("buildVaporizer");
    }
    return true;
}


std::string
GNEAdditionalHandler::getFileName(const SUMOSAXAttributes& attrs, const std::string& base, const bool allowEmpty) {
    // get the file name to read further definitions from
    bool ok = true;
    std::string file = attrs.getOpt<std::string>(SUMO_ATTR_FILE, 0, ok, "");
    if (file == "") {
        if (allowEmpty) {
            return file;
        }
        WRITE_WARNING("No filename given.");
    }
    // check whether absolute or relative filenames are given
    if (!FileHelpers::isAbsolute(file)) {
        return FileHelpers::getConfigurationRelative(base, file);
    }
    return file;
}


double
GNEAdditionalHandler::getPosition(double pos, GNELane& lane, bool friendlyPos , const std::string& additionalID) {
    if (pos < 0) {
        pos = lane.getLaneShapeLength() + pos;
    }
    if (pos > lane.getLaneShapeLength()) {
        if (friendlyPos) {
            pos = lane.getLaneShapeLength() - (double) 0.1;
        } else {
            WRITE_WARNING("The position of additional '" + additionalID + "' lies beyond the lane's '" + lane.getID() + "' length.");
        }
    }
    return pos;
}


bool
GNEAdditionalHandler::fixStoppinPlacePosition(std::string& startPos, std::string& endPos, const double laneLength, const double minLength, const bool friendlyPos) {
    // first check if startPos and endPos were defined
    if (GNEAttributeCarrier::canParse<double>(startPos) && GNEAttributeCarrier::canParse<double>(endPos)) {
        // first parse strings to numerical values
        double startPosDouble = GNEAttributeCarrier::parse<double>(startPos);
        double endPosDouble = GNEAttributeCarrier::parse<double>(endPos);

        // fix both positions
        if (minLength > laneLength) {
            return false;
        }
        if (startPosDouble < 0) {
            startPosDouble += laneLength;
        }
        if (endPosDouble < 0) {
            endPosDouble += laneLength;
        }
        if ((endPosDouble < minLength) || (endPosDouble > laneLength)) {
            if (!friendlyPos) {
                return false;
            }
            if (endPosDouble < minLength) {
                endPosDouble = minLength;
            }
            if (endPosDouble > laneLength) {
                endPosDouble = laneLength;
            }
        }
        if ((startPosDouble < 0) || (startPosDouble > (endPosDouble - minLength))) {
            if (!friendlyPos) {
                return false;
            }
            if (startPosDouble < 0) {
                startPosDouble = 0;
            }
            if (startPosDouble > (endPosDouble - minLength)) {
                startPosDouble = (endPosDouble - minLength);
            }
        }
        // Modify original positions
        startPos = toString(startPosDouble);
        endPos = toString(endPosDouble);
    } else if (GNEAttributeCarrier::canParse<double>(startPos)) {
        // check that endPosition is valid
        if(endPos.empty() || !GNEAttributeCarrier::canParse<double>(endPos)) {
            // first parse to double only startPos
            double startPosDouble = GNEAttributeCarrier::parse<double>(startPos);
            // fix both positions
            if (minLength > laneLength) {
                return false;
            }
            if (startPosDouble < 0) {
                startPosDouble += laneLength;
            }
            if ((startPosDouble < 0) || startPosDouble > (laneLength - minLength)) {
                if (!friendlyPos) {
                    return false;
                }
                if (startPosDouble < 0) {
                    startPosDouble = 0;
                }
                if (startPosDouble > (laneLength - minLength)) {
                    startPosDouble = (laneLength - minLength);
                }
            }
            // Modify only start position
            startPos = toString(startPosDouble);
        } else {
            return false;
        }
    } else if (GNEAttributeCarrier::canParse<double>(endPos)) {
        // check that endPosition is valid
        if(startPos.empty() || !GNEAttributeCarrier::canParse<double>(startPos)) {
            // first parse to double only endPos
            double endPosDouble = GNEAttributeCarrier::parse<double>(endPos);
            // fix both positions
            if (minLength > laneLength) {
                return false;
            }
            if (endPosDouble < 0) {
                endPosDouble += laneLength;
            }
            if ((endPosDouble < minLength) || (endPosDouble > laneLength)) {
                if (!friendlyPos) {
                    return false;
                }
                if (endPosDouble < minLength) {
                    endPosDouble = minLength;
                }
                if (endPosDouble > laneLength) {
                    endPosDouble = laneLength;
                }
            }
            if (0 > (endPosDouble - minLength)) {
                if (!friendlyPos) {
                    return false;
                }
            }
            // Modify only end position
            endPos = toString(endPosDouble);
        } else {
            return false;
        }
    }
    return true;
}


bool GNEAdditionalHandler::checkAndFixDetectorPositionPosition(double& pos, const double laneLength, const bool friendlyPos) {
    if ((pos < 0) || (pos > laneLength)) {
        if (!friendlyPos) {
            return false;
        } else if (pos < 0) {
            pos = 0;
        } else if (pos > laneLength) {
            pos = laneLength;
        }
    }
    return true;
}


bool GNEAdditionalHandler::fixE2DetectorPositionPosition(double& pos, double& length, const double laneLength, const bool friendlyPos) {
    if ((pos < 0) || ((pos + length) > laneLength)) {
        if (!friendlyPos) {
            return false;
        } else if (pos < 0) {
            pos = 0;
        } else if (pos > laneLength) {
            pos = laneLength;
            length = 0;
        } else if ((pos + length) > laneLength) {
            length = laneLength - pos;
        }
    }
    return true;
}


bool 
GNEAdditionalHandler::accessCanBeCreated(GNEAdditional *busStopParent, GNEEdge &edge) {
    // check that busStopParent is a busStop
    assert(busStopParent->getTag() == SUMO_TAG_BUS_STOP);
    // check if exist another acces for the same busStop in the given edge
    for (auto i : busStopParent->getAdditionalChilds()) {
        for (auto j : edge.getLanes()) {
            if (i->getAttribute(SUMO_ATTR_LANE) == j->getID()) {
                return false;
            }
        }
    }
    return true;
}


bool 
GNEAdditionalHandler::checkOverlappingRerouterIntervals(GNEAdditional *rerouter, double newBegin, double newEnd) {
    // check that rerouter is correct
    assert(rerouter->getTag() == SUMO_TAG_REROUTER);
    // declare a vector to keep sorted rerouter childs
    std::vector<std::pair<double, double>> sortedIntervals;
    // iterate over additional childs
    for (auto i : rerouter->getAdditionalChilds()) {
        sortedIntervals.push_back(std::make_pair(0.,0.));
        // set begin and end
        sortedIntervals.back().first = GNEAttributeCarrier::parse<double>(i->getAttribute(SUMO_ATTR_BEGIN));
        sortedIntervals.back().second = GNEAttributeCarrier::parse<double>(i->getAttribute(SUMO_ATTR_END));
    }
    // add new intervals
    sortedIntervals.push_back(std::make_pair(newBegin, newEnd));
    // sort childs
    std::sort(sortedIntervals.begin(), sortedIntervals.end());
    // check overlapping after sorting
    for (int i = 0; i < (sortedIntervals.size() - 1); i++) {
        if(sortedIntervals.at(i).second > sortedIntervals.at(i+1).first) {
            return false;
        }
    }
    return true;
}


void 
GNEAdditionalHandler::HierarchyInsertedElements::insertElement(SumoXMLTag tag) {
    myInsertedElements.push_back(std::make_pair(tag, ""));
}


void 
GNEAdditionalHandler::HierarchyInsertedElements::commitElementInsertion (const std::string &id) {
    myInsertedElements.back().second = id;
}


void 
GNEAdditionalHandler::HierarchyInsertedElements::popElement() {
    if(!myInsertedElements.empty()) {
        myInsertedElements.pop_back();
    }
}


GNEAdditional*
GNEAdditionalHandler::HierarchyInsertedElements::retrieveAdditionalParent(GNEViewNet* viewNet, SumoXMLTag expectedTag) const {
    if(myInsertedElements.size() < 2) {
        // currently we're finding additional parent in the additional XML root
        WRITE_WARNING("A " + toString(myInsertedElements.back().first) + " must be declared within the definition of a " + toString(expectedTag) + ".");
        return nullptr;
    } else {
        GNEAdditional *retrievedAdditional = viewNet->getNet()->retrieveAdditional((myInsertedElements.end()-2)->first, (myInsertedElements.end()-2)->second, false);
        if(retrievedAdditional == nullptr) {
            // additional doesn't exist
            WRITE_WARNING("A " + toString((myInsertedElements.end()-2)->first) + " must be declared within the definition of a " + toString(expectedTag) + ".");
            return nullptr;
        } else if( retrievedAdditional->getTag() != expectedTag) {
            // invalid additional parent
            WRITE_WARNING("A " + toString((myInsertedElements.end()-2)->first) + " cannot be declared within the definition of a " + toString(retrievedAdditional->getTag()) + ".");
            return nullptr;
        } else {
            return retrievedAdditional;
        }
    }
}

/****************************************************************************/
