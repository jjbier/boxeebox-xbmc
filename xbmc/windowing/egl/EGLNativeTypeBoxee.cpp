/*
 *      Copyright (C) 2011-2012 Team XBMC
 *      http://www.xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */
#include "system.h"
#include <EGL/egl.h>
#include "EGLNativeTypeBoxee.h"
#include "guilib/gui3d.h"
#include "cores/IntelSMDGlobals.h"
#include "utils/log.h"

CEGLNativeTypeBoxee::CEGLNativeTypeBoxee()
{
}

CEGLNativeTypeBoxee::~CEGLNativeTypeBoxee()
{
} 

bool CEGLNativeTypeBoxee::CheckCompatibility()
{
#if defined(TARGET_BOXEE)
  return true;
#endif
  return false;
}

void CEGLNativeTypeBoxee::Initialize()
{
  gdl_init();
  int enable = 1;

  gdl_rectangle_t srcRect, dstRect;
  gdl_display_info_t   display_info;
  gdl_get_display_info(GDL_DISPLAY_ID_0, &display_info);

  dstRect.origin.x = 0;
  dstRect.origin.y = 0;
  dstRect.width = display_info.tvmode.width;
  dstRect.height = display_info.tvmode.height;

  srcRect.origin.x = 0;
  srcRect.origin.y = 0;
  srcRect.width = display_info.tvmode.width;
  srcRect.height = display_info.tvmode.height;


  gdl_port_set_attr(GDL_PD_ID_HDMI, GDL_PD_ATTR_ID_POWER, &enable);
  gdl_plane_reset(GDL_GRAPHICS_PLANE);
  gdl_plane_config_begin(GDL_GRAPHICS_PLANE);
  gdl_plane_set_uint(GDL_PLANE_SRC_COLOR_SPACE, GDL_COLOR_SPACE_RGB);
  gdl_plane_set_uint(GDL_PLANE_PIXEL_FORMAT, GDL_PF_ARGB_32);
  gdl_plane_set_rect(GDL_PLANE_DST_RECT, &dstRect);
  gdl_plane_set_rect(GDL_PLANE_SRC_RECT, &srcRect);
  gdl_plane_config_end(GDL_FALSE);
  return;
}
void CEGLNativeTypeBoxee::Destroy()
{
  gdl_close();
  return;
}

bool CEGLNativeTypeBoxee::CreateNativeDisplay()
{
  m_nativeDisplay = EGL_DEFAULT_DISPLAY;
  return true;
}

bool CEGLNativeTypeBoxee::CreateNativeWindow()
{
#if defined(TARGET_BOXEE)
  m_nativeWindow = (void*)GDL_GRAPHICS_PLANE;
  return true;
#else
  return false;
#endif
}  

bool CEGLNativeTypeBoxee::GetNativeDisplay(XBNativeDisplayType **nativeDisplay) const
{
  if (!nativeDisplay)
    return false;
  *nativeDisplay = (XBNativeDisplayType*) &m_nativeDisplay;
  return true;
}

bool CEGLNativeTypeBoxee::GetNativeWindow(XBNativeWindowType **nativeWindow) const
{
  if (!nativeWindow || !m_nativeWindow)
    return false;
  *nativeWindow = (XBNativeWindowType*) &m_nativeWindow;
  return true;
}

bool CEGLNativeTypeBoxee::DestroyNativeDisplay()
{
  return true;
}

bool CEGLNativeTypeBoxee::DestroyNativeWindow()
{
  m_nativeWindow = NULL;
  return true;
}

bool CEGLNativeTypeBoxee::GetNativeResolution(RESOLUTION_INFO *res) const
{
#if defined(TARGET_BOXEE)
  gdl_display_info_t   display_info;
  gdl_get_display_info(GDL_DISPLAY_ID_0, &display_info);

  res->iWidth = display_info.tvmode.width;
  res->iHeight= display_info.tvmode.height;

  switch (display_info.tvmode.refresh)
  {
    case GDL_REFRESH_23_98: res->fRefreshRate = 23.98; break;
    case GDL_REFRESH_24:    res->fRefreshRate = 24;    break;
    case GDL_REFRESH_25:    res->fRefreshRate = 25;    break;
    case GDL_REFRESH_29_97: res->fRefreshRate = 29.97; break;
    case GDL_REFRESH_30:    res->fRefreshRate = 30;    break;
    case GDL_REFRESH_50:    res->fRefreshRate = 50;    break;
    case GDL_REFRESH_59_94: res->fRefreshRate = 59.94; break;
    case GDL_REFRESH_60:    res->fRefreshRate = 60;    break;
    case GDL_REFRESH_48:    res->fRefreshRate = 48;    break;
    case GDL_REFRESH_47_96: res->fRefreshRate = 47.96; break;
    default:                res->fRefreshRate = 0;     break;
  }

  if (display_info.tvmode.interlaced)
  {
    res->dwFlags = D3DPRESENTFLAG_INTERLACED;
  }
  else
  { 
    res->dwFlags = D3DPRESENTFLAG_PROGRESSIVE;
  }
  res->iScreen       = 0;
  res->bFullScreen   = true;
  res->iSubtitles    = (int)(0.965 * res->iHeight);
  res->fPixelRatio   = 1.0f;
  res->iScreenWidth  = res->iWidth;
  res->iScreenHeight = res->iHeight;
  res->strMode.Format("%dx%d @ %.2f%s - Full Screen", res->iScreenWidth, res->iScreenHeight, res->fRefreshRate,
  res->dwFlags & D3DPRESENTFLAG_INTERLACED ? "i" : "");
  CLog::Log(LOGNOTICE,"Current resolution: %s\n",res->strMode.c_str());
  return true;
#else
  return false;
#endif
}

bool CEGLNativeTypeBoxee::SetNativeResolution(const RESOLUTION_INFO &res)
{
  return false;
}

bool CEGLNativeTypeBoxee::ProbeResolutions(std::vector<RESOLUTION_INFO> &resolutions)
{
  RESOLUTION_INFO res;
  bool ret = false;
  ret = GetNativeResolution(&res);
  if (ret && res.iWidth > 1 && res.iHeight > 1)
  {
    resolutions.push_back(res);
    return true;
  }
  return false;
}

bool CEGLNativeTypeBoxee::GetPreferredResolution(RESOLUTION_INFO *res) const
{
  return false;
}

bool CEGLNativeTypeBoxee::ShowWindow(bool show)
{
  return false;
}
