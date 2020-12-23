/* See LICENSE file for copyright and license details. */

#include "base16.h"

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "Source Sans Pro:size=10" };
static const char *colors[][3]      = {
	/*               fg                    bg                    border */
	[SchemeNorm] = { base16onedark.base05, base16onedark.base00, base16onedark.base00 },
	[SchemeSel]  = { base16onedark.base07, base16onedark.base01, base16onedark.base07 },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ NULL,       NULL,       NULL,       0,            0,           -1 },
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
	"-nb", base16onedark.base01,
	"-nf", base16onedark.base05,
	"-sb", base16onedark.base02,
	"-sf", base16onedark.base07,
	"-bc", base16onedark.base01,
	NULL
};

static const char *termcmd[]  = { "st", NULL };

static const char *volup[] = {"pactl", "set-sink-volume", "@DEFAULT_SINK@", "-2%", NULL};
static const char *voldown[] = {"pactl", "set-sink-volume", "@DEFAULT_SINK@", "-2%", NULL};
static const char *volmute[] = {"pactl", "set-sink-volume", "@DEFAULT_SINK@", "toggle", NULL};

static Key keys[] = {
	/* modifier         key                      function        argument */
	{ MODKEY,           XK_d,                    spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask, XK_Return,               spawn,          {.v = termcmd } },
	{ MODKEY,           XK_b,                    togglebar,      {0} },
	{ MODKEY,           XK_n,                    focusstack,     {.i = +1 } },
	{ MODKEY,           XK_p,                    focusstack,     {.i = -1 } },
	{ MODKEY,           XK_Up,                   incnmaster,     {.i = +1 } },
	{ MODKEY,           XK_Down,                 incnmaster,     {.i = -1 } },
	{ MODKEY,           XK_minus,                setmfact,       {.f = -0.05} },
	{ MODKEY,           XK_equal,                setmfact,       {.f = +0.05} },
	{ MODKEY,           XK_Return,               zoom,           {0} },
	{ MODKEY,           XK_Tab,                  view,           {0} },
	{ MODKEY|ShiftMask, XK_q,                    killclient,     {0} },
	{ MODKEY,           XK_t,                    setlayout,      {.v = &layouts[0]} },
	{ MODKEY,           XK_f,                    setlayout,      {.v = &layouts[1]} },
	{ MODKEY,           XK_m,                    setlayout,      {.v = &layouts[2]} },
	{ MODKEY|ShiftMask, XK_space,                togglefloating, {0} },
	{ MODKEY,           XK_0,                    view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask, XK_0,                    tag,            {.ui = ~0 } },
	{ MODKEY,           XK_comma,                focusmon,       {.i = -1 } },
	{ MODKEY,           XK_period,               focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask, XK_comma,                tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask, XK_period,               tagmon,         {.i = +1 } },
	TAGKEYS(            XK_1,                                    0),
	TAGKEYS(            XK_2,                                    1),
	TAGKEYS(            XK_3,                                    2),
	TAGKEYS(            XK_4,                                    3),
	TAGKEYS(            XK_5,                                    4),
	TAGKEYS(            XK_6,                                    5),
	TAGKEYS(            XK_7,                                    6),
	TAGKEYS(            XK_8,                                    7),
	TAGKEYS(            XK_9,                                    8),
	{ MODKEY|ShiftMask, XK_x,                    quit,           {0} },
	{ 0,                XF86XK_AudioRaiseVolume, spawn,          {.v = volup } },
	{ 0,                XF86XK_AudioLowerVolume, spawn,          {.v = voldown } },
	{ 0,                XF86XK_AudioMute,        spawn,          {.v = volmute } },
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

static const char startup[] = "$HOME/scripts/startup.sh";
