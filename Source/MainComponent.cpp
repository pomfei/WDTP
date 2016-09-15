﻿/*
  ==============================================================================

    MainComponent.cpp
    Created: 4 Sep 2016 11:08:17am
    Author:  SwingCoder

  ==============================================================================
*/

#include "WdtpHeader.h"

extern PropertiesFile* systemFile;

//==============================================================================
MainContentComponent::MainContentComponent() 
{
    // must be these order...
    addAndMakeVisible (editAndPreview = new EditAndPreview ());
    addAndMakeVisible (fileTree = new FileTreeContainer (editAndPreview));
    addAndMakeVisible (toolBar = new TopToolBar (fileTree, editAndPreview));
    addAndMakeVisible (layoutBar = new StretchableLayoutResizerBar (&layoutManager, 1, true));

    // stretched layout. arg: index, min-width, max-width，default x%
    layoutManager.setItemLayout (0, 2, -0.3, -0.23);  // fileTree
    layoutManager.setItemLayout (1, 3, 3, 3);            // layoutBar
    layoutManager.setItemLayout (2, -0.7, -1.0, -0.77);   // editAndPreview

    setSize (1200, 720);    
}

//=======================================================================
MainContentComponent::~MainContentComponent()
{

}
//=========================================================================
void MainContentComponent::paint (Graphics& g)
{
    g.fillAll (Colour(0xffdcdbdb));
    //g.fillAll (Colours::darkgrey);
}

//=========================================================================
void MainContentComponent::resized()
{
    toolBar->setBounds (0, 0, getWidth (), 45);

    // stretched layout
    Component* comps[] = { fileTree, layoutBar, editAndPreview };
    layoutManager.layOutComponents (comps, 3, 0, 45, getWidth (), getHeight () - 45, false, true);
}

//=================================================================================================
MainWindow::MainWindow (String name) : DocumentWindow (name,
                                                       Colours::lightgrey,
                                                       DocumentWindow::allButtons)
{
    setUsingNativeTitleBar (true);
    setContentOwned (mainComp = new MainContentComponent (), true);
    setResizable (true, false);
    setResizeLimits (800, 500, 3200, 2400);

    centreWithSize (getWidth (), getHeight ());
    setVisible (true);
}

//=================================================================================================
void MainWindow::closeButtonPressed ()
{
    mainComp->getFileTree ()->saveDocAndProject ();
    JUCEApplication::getInstance ()->systemRequestedQuit ();
}

//=================================================================================================
void MainWindow::openProject (const File& projectFile)
{
    mainComp->getFileTree ()->openProject (projectFile);
}