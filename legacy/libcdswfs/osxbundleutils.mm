//
//  osxbundleutils.m
//  CdsWfs
//
//  Created by Simone Campanini on 29/03/16.
//  Copyright © 2016 Fons Adriaensen. All rights reserved.
//

#import <cstring>
#import <fcntl.h>
#import <errno.h>

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#import "osxbundleutils.h"

WfsMacBundle* WfsMacBundle::m_pSharedInstance = 0;

WfsMacBundle* WfsMacBundle::sharedInstance()
{
    if (m_pSharedInstance == 0)
    {
        m_pSharedInstance = new WfsMacBundle();
    }
    return m_pSharedInstance;
}

bool WfsMacBundle::fileExists(const char* fn)
{
    int fd = open(fn, O_RDONLY);
    
    if (fd == 0)
    {
        if (errno == ENOENT)
        {
            fprintf(stderr, "File: %s does not exist.\n", fn);
        }
        else
        {
            fprintf(stderr, "Cannot access file '%s': %s.\n",
                    fn,
                    strerror(errno));
        }
        return false;
    }
    close(fd);
    return true;
}

const char* WfsMacBundle::findImagePath(const char* name)
{
    char* path = new char [512];
    snprintf(path, 508, "%s/%s", m_resourcesPath, name);
    
    if (strstr(name, ".png") == 0)
    {
        strcat(path, ".png");
    }

    if (! fileExists(path))
    {
        delete [] path;
        return NULL;
    }
    
    printf("Found image: %s\n", path);

    return path;
}

const char* WfsMacBundle::findPluginPath(const char* name)
{
    char* path = new char [512];
    
    snprintf(path, 504, "%s/%s", m_pluginsPath, name);
    
    printf ("Searching plugin in bundle: %s\n", path);
    if (strstr(name, ".dylib") == 0)
    {
        strcat(path, ".dylib");
    }
    
    if (! fileExists(path))
    {
        snprintf(path, 504, "%s/Library/Audio/Plug-Ins/Wfs/%s",
                 getenv("HOME"), name);
        
        if (strstr(name, ".dylib") == 0)
        {
            strcat(path, ".dylib");
        }
        
        if (! fileExists(path))
        {
            snprintf(path, 504, "/opt/local/lib/%s", name);
            
            if (strstr(name, ".dylib") == 0)
            {
                strcat(path, ".dylib");
            }

            if (! fileExists(path))
            {
                delete [] path;
                return NULL;
            }
        }
    }
    
    printf("Found plugin: %s\n", path);
    
    return path;
}

const char* WfsMacBundle::findExecutabelePath(const char* name)
{
    char* path = new char [512];
    snprintf(path, 508, "%s/%s", m_resourcesPath, name);
    
    if (! fileExists(path))
    {
        delete [] path;
        return NULL;
    }
    
    printf("Found executable: %s\n", path);
    
    return path;
}

const char* WfsMacBundle::findBundledExecutablePath(const char* name)
{
    char* path = new char [512];
    snprintf(path, 508, "%s/%s.app/Contents/MacOS/%s", m_resourcesPath, name, name);
    
    if (! fileExists(path))
    {
        delete [] path;
        return NULL;
    }
    
    printf("Found bundled executable: %s\n", path);
    
    return path;
}

WfsMacBundle::WfsMacBundle()
{
    NSBundle* bundle = [NSBundle mainBundle];
    
    [[bundle resourcePath] getCString:m_resourcesPath
                            maxLength:512
                             encoding:NSASCIIStringEncoding];

    strcpy(m_pluginsPath, m_resourcesPath);
    char* c  = strrchr(m_pluginsPath, '/');
    c++; *c = 0;
    strcat(m_pluginsPath, "PlugIns");
    
    printf("Resources path: %s\n", m_resourcesPath);
    printf("Plugins path: %s\n", m_pluginsPath);
}

