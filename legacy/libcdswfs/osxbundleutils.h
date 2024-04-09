//
//  osxbundleutils.h
//  CdsWfs
//
//  Created by Simone Campanini on 29/03/16.
//  Copyright © 2016 Fons Adriaensen. All rights reserved.
//

#ifndef osxbundleutils_h
#define osxbundleutils_h


class WfsMacBundle
{
    char m_resourcesPath [512];
    char m_pluginsPath   [512];
    
    bool fileExists(const char* fn);
    
  public:
    static WfsMacBundle* m_pSharedInstance;
    static WfsMacBundle* sharedInstance();
    
    const char* findImagePath(const char* name);
    const char* findPluginPath(const char* name);
    const char* findExecutabelePath(const char* name);
    const char* findBundledExecutablePath(const char* name);
    
    WfsMacBundle();
};

#endif /* osxbundleutils_h */
