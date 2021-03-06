/*
  ==============================================================================

    TipsBank.cpp
    Created: 30 Mar 2017 8:08:24pm
    Author:  SwingCoder

  ==============================================================================
*/

#include "WdtpHeader.h"

TipsBank::TipsBank() 
    : Thread ("TipsBankThread")
{

}

//=================================================================================================
TipsBank::~TipsBank()
{
    stopThread (3000);
    clearSingletonInstance();
}

juce_ImplementSingleton (TipsBank);

//=================================================================================================
void TipsBank::rebuildTipsBank()
{
    if (FileTreeContainer::projectTree.isValid() 
        && FileTreeContainer::projectFile.existsAsFile()
        && !isThreadRunning())
        startThread();
}

//=================================================================================================
const bool TipsBank::addNewTip (const String& name, const String& content)
{
    if (tipsBank.contains (name))
        return false;

    tipsBank.set (name, content);
    return true;
}

//=================================================================================================
const bool TipsBank::hasThisKey (const String& keyStr) const
{
    for (HashMap<String, String>::Iterator itr (tipsBank); itr.next(); )
    {
        if (itr.getKey().contains (keyStr))
            return true;
    }

    return false;
}

//=================================================================================================
void TipsBank::run()
{
    tipsBank.clear();
    const File& tipsFile (FileTreeContainer::projectFile.getSiblingFile ("docs").getChildFile ("tips.md"));

    // from tips doc
    if (tipsFile.existsAsFile())
    {
        StringArray strs;
        strs.addLines (tipsFile.loadFileAsString());
        strs.removeEmptyStrings (true);
        strs.trim();
        
        // only extrct the content which matched tips format
        for (int i = strs.size(); --i >= 0; )
        {
            if (strs[i].substring (0, 6) != "    - " && strs[i].substring (0, 2) != "- ")
                strs.remove (i);

            else if (strs[i].substring (0, 6) == "    - ")
                strs.getReference (i) = strs[i].substring (6);

            else if (strs[i].substring (0, 2) == "- ")
                strs.getReference (i) = strs[i].substring (2);
        }

        // note: if more than one of tips has the same key
        // it'll only keep the last one
        for (int i = 0; i < strs.size() - 1; i += 2)
        {
            const String& key (strs[i]);
            const String& value (strs[i + 1]);

            tipsBank.set (key, value);
        }        
    }

    // from project files
    tipsFromProjectFiles (FileTreeContainer::projectTree);

    /*for (HashMap<String, String>::Iterator i (tipsBank); i.next();)
        DBG (i.getKey() << " -> " << i.getValue());*/
}

//=================================================================================================
void TipsBank::tipsFromProjectFiles (ValueTree tree)
{
    String&& title (tree.getProperty ("title").toString());
    ValueTree parentTree = tree.getParent();

    while (parentTree.isValid() && parentTree.getType().toString() == "dir")
    {
        title = parentTree.getProperty ("title").toString() + "/" + title;
        parentTree = parentTree.getParent();
    }

    String filePath (DocTreeViewItem::getHtmlFile (tree).getFullPathName());
    filePath = filePath.fromFirstOccurrenceOf (FileTreeContainer::projectFile.getSiblingFile ("site")
                                               .getFullPathName(), false, false);
    tipsBank.set (title, "@ " + filePath.substring (1).replace ("\\", "/"));

    for (int i = tree.getNumChildren(); --i >= 0; )
        tipsFromProjectFiles (tree.getChild (i));
}
