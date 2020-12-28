/* See LICENSE file for copyright and license details. */

#include "base16.h"

/* appearance */
static const unsigned int borderpx    = 0;          /* border pixel of windows */
static const unsigned int gappx       = 20;         /* gaps between windows */
static const unsigned int snap        = 10;         /* snap pixel */
static const int showbar              = 1;          /* 0 means no bar */
static const int topbar               = 1;          /* 0 means bottom bar */
static const unsigned int barheight   = gappx + 30; /* height of bar */
static const char *fonts[]            = { "Roboto Mono:size=12" };
static const Base16Colors base16theme = base16gruvboxdarkmedium;
static const char *colors[][3]        = {
	/*               fg                  bg                  border */
	[SchemeNorm] = { base16theme.base05, base16theme.base00, base16theme.base00 },
	[SchemeSel]  = { base16theme.base07, base16theme.base01, base16theme.base07 },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class  instance  title  tags mask  iscentered,  isfloating  monitor */
	{ NULL,   NULL,     NULL,  0,         0,           0,          -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "Tile",     tile },    /* first entry is default */
	{ "Float",    NULL },    /* no layout function means floating behavior */
	{ "Mono",     monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} }

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { /* requires custom build of dmenu */
	"dmenu_run",
	"-c",
	"-l",  "5",
	"-h",  "40",
	"-bw", "60",
	"-fn", "Source Sans Pro:size=12",
	"-m",  dmenumon,
	"-nb", base16theme.base01,
	"-nf", base16theme.base05,
	"-sb", base16theme.base02,
	"-sf", base16theme.base07,
	"-bc", base16theme.base01,
	NULL
};

static const char *termcmd[]  = { "st", NULL };

static const char *volup[] = {"pactl", "set-sink-volume", "@DEFAULT_SINK@", "+2%", NULL};
static const char *voldown[] = {"pactl", "set-sink-volume", "@DEFAULT_SINK@", "-2%", NULL};
static const char *volmute[] = {"pactl", "set-sink-mute", "@DEFAULT_SINK@", "toggle", NULL};

static const char *brightup[] = {"xbacklight", "+5", NULL};
static const char *brightdown[] = {"xbacklight", "-5", NULL};

static const char *screenshot[] = {"/bin/bash", "-c", "~/scripts/screenshot.sh", NULL};
static const char *screenshotregion[] = {"/bin/bash", "-c", "~/scripts/screenshot.sh region", NULL};

static Key keys[] = {
	/* modifier           key                       function        argument */
	{ MODKEY,             XK_d,                     spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,   XK_Return,                spawn,          {.v = termcmd } },
	{ MODKEY,             XK_b,                     togglebar,      {0} },
	{ MODKEY,             XK_n,                     focusstack,     {.i = +1 } },
	{ MODKEY,             XK_p,                     focusstack,     {.i = -1 } },
	{ MODKEY,             XK_Up,                    incnmaster,     {.i = +1 } },
	{ MODKEY,             XK_Down,                  incnmaster,     {.i = -1 } },
	{ MODKEY,             XK_minus,                 setmfact,       {.f = -0.05} },
	{ MODKEY,             XK_equal,                 setmfact,       {.f = +0.05} },
	{ MODKEY,             XK_Return,                zoom,           {0} },
	{ MODKEY,             XK_Tab,                   view,           {0} },
	{ MODKEY|ShiftMask,   XK_q,                     killclient,     {0} },
	{ MODKEY,             XK_t,                     setlayout,      {.v = &layouts[0]} },
	{ MODKEY,             XK_f,                     setlayout,      {.v = &layouts[1]} },
	{ MODKEY,             XK_m,                     setlayout,      {.v = &layouts[2]} },
	{ MODKEY|ShiftMask,   XK_space,                 togglefloating, {0} },
	{ MODKEY,             XK_0,                     view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,   XK_0,                     tag,            {.ui = ~0 } },
	{ MODKEY,             XK_comma,                 focusmon,       {.i = -1 } },
	{ MODKEY,             XK_period,                focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,   XK_comma,                 tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,   XK_period,                tagmon,         {.i = +1 } },
	TAGKEYS(              XK_1,                                     0),
	TAGKEYS(              XK_2,                                     1),
	TAGKEYS(              XK_3,                                     2),
	TAGKEYS(              XK_4,                                     3),
	TAGKEYS(              XK_5,                                     4),
	TAGKEYS(              XK_6,                                     5),
	TAGKEYS(              XK_7,                                     6),
	TAGKEYS(              XK_8,                                     7),
	TAGKEYS(              XK_9,                                     8),
	{ MODKEY|ShiftMask,   XK_x,                     quit,           {0} },
	{ MODKEY,             XK_g,                     setgaps,        {.i = 0 } },
	{ MODKEY|ControlMask, XK_g,                     setgaps,        {.i = -5 } },
	{ MODKEY|ShiftMask,   XK_g,                     setgaps,        {.i = +5 } },
	{ 0,                  XF86XK_AudioRaiseVolume,  spawn,          {.v = volup } },
	{ 0,                  XF86XK_AudioLowerVolume,  spawn,          {.v = voldown } },
	{ 0,                  XF86XK_AudioMute,         spawn,          {.v = volmute } },
	{ 0,                  XF86XK_MonBrightnessUp,   spawn,          {.v = brightup } },
	{ 0,                  XF86XK_MonBrightnessDown, spawn,          {.v = brightdown } },
	{ 0,                  XK_Print,                 spawn,          {.v = screenshot } },
	{ ControlMask,        XK_Print,                 spawn,          {.v = screenshotregion } },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

static const char *ipcsockpath = "/tmp/dwm.sock";
static IPCCommand ipccommands[] = {
	IPCCOMMAND(  view,                1,      {ARG_TYPE_UINT}   ),
	IPCCOMMAND(  toggleview,          1,      {ARG_TYPE_UINT}   ),
	IPCCOMMAND(  tag,                 1,      {ARG_TYPE_UINT}   ),
	IPCCOMMAND(  toggletag,           1,      {ARG_TYPE_UINT}   ),
	IPCCOMMAND(  tagmon,              1,      {ARG_TYPE_UINT}   ),
	IPCCOMMAND(  focusmon,            1,      {ARG_TYPE_SINT}   ),
	IPCCOMMAND(  focusstack,          1,      {ARG_TYPE_SINT}   ),
	IPCCOMMAND(  zoom,                1,      {ARG_TYPE_NONE}   ),
	IPCCOMMAND(  incnmaster,          1,      {ARG_TYPE_SINT}   ),
	IPCCOMMAND(  killclient,          1,      {ARG_TYPE_SINT}   ),
	IPCCOMMAND(  togglefloating,      1,      {ARG_TYPE_NONE}   ),
	IPCCOMMAND(  setmfact,            1,      {ARG_TYPE_FLOAT}  ),
	IPCCOMMAND(  setlayoutsafe,       1,      {ARG_TYPE_PTR}    ),
	IPCCOMMAND(  quit,                1,      {ARG_TYPE_NONE}   )
};

static const char startup[] = "$HOME/scripts/startup.sh";
