#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    test.py
# @author  Pablo Alvarez Lopez
# @date    2016-11-25
# @version $Id$

# import common functions for netedit tests
import os
import sys

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, match = netedit.setupAndStart(neteditTestRoot)

# go to select mode
netedit.selectMode()

# select all using invert
netedit.selectionInvert()

# go to inspect mode
netedit.inspectMode()

# inspect E1
netedit.leftClick(match, 150, 240)

# Change parameter 0 with a non valid value (non numeral)
netedit.modifyAttribute(0, "dummyFrequency")

# Change parameter 0 with a non valid value (negative)
netedit.modifyAttribute(0, "-100")

# Change parameter 0 with a valid value
netedit.modifyAttribute(0, "120")

# Change parameter 1 with an empty value
netedit.modifyAttribute(1, "")

# Change parameter 1 with an non valid value
netedit.modifyAttribute(1, "%%%&&%$%$")

# Change parameter 1 with a duplicated value
netedit.modifyAttribute(1, "e1Detector_gneE2_1_1.txt")

# Change parameter 1 with a valid value
netedit.modifyAttribute(1, "myOwnOutput.txt")

# Change parameter 2 with an invalid value
netedit.modifyAdditionalDefaultValue(2, "DummyVClasses")

# Change parameter 2
netedit.modifyAdditionalDefaultValue(2, "private passenger taxi bus")

# Change boolean parameter 3
netedit.modifyBoolAttribute(3)

# Check undos and redos
netedit.undo(match, 4)
netedit.redo(match, 4)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
