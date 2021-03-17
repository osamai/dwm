/* See LICENSE file for copyright and license details. */

#include "movestack.c"

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int gappih    = 10;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const unsigned int gappoh    = 20;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 20;       /* vert outer gap between windows and screen edge */
static       int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=10" };
static const char dmenufont[]       = "monospace:size=10";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char *colors[][3]      = {
	/*                fg         bg         border   */
	[SchemeNorm]  = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]   = { col_gray4, col_cyan,  col_cyan  },
	[SchemeTitle] = { col_gray4, col_gray1, col_cyan  },
};

typedef struct {
	const char *name;
	const void *cmd;
} Sp;
const char *spcmd1[] = {"st", "-n", "spterm", "-g", "120x34", NULL };
static Sp scratchpads[] = {
	/* name          cmd  */
	{"spterm",      spcmd1},
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,        NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,        NULL,       1 << 8,       0,           -1 },
	{ "Chromium", NULL,        NULL,       1 << 8,       0,           -1 },
	{ NULL,       "spterm",    NULL,       SPTAG(0),     1,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "TTT",      bstack },
	{ "[@]",      spiral },
	{ "[\\]",     dwindle },
	{ "[D]",      deck },
	{ "[M]",      monocle },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define ALTKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "st", NULL };

static Key keys[] = {
	/* modifier                     key           function        argument */
	{ MODKEY,                       XK_p,         spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_Return,    spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,         togglebar,      {0} },
	{ MODKEY,                       XK_j,         focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,         focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_o,         incnmaster,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_o,         incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,         setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,         setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_j,         movestack,      {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,         movestack,      {.i = -1 } },
	{ MODKEY,                       XK_space,     zoom,           {0} },
	{ MODKEY|ShiftMask,             XK_space,     togglefloating, {0} },
	{ MODKEY,                       XK_s,         togglesticky,   {0} },
	{ MODKEY,                       XK_f,         togglefullscr,  {0} },
	{ MODKEY,                       XK_a,         togglegaps,     {0} },
	{ MODKEY|ShiftMask,             XK_a,         defaultgaps,    {0} },
	{ MODKEY,                       XK_z,         incrgaps,       {.i = +3 } },
	{ MODKEY|ShiftMask,             XK_z,         incrgaps,       {.i = -3 } },
	{ MODKEY,                       XK_Tab,       view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,         killclient,     {0} },
	{ MODKEY,                       XK_t,         setlayout,      {.v = &layouts[0]} }, /* tile */
	{ MODKEY|ShiftMask,             XK_t,         setlayout,      {.v = &layouts[1]} }, /* bstack */
	{ MODKEY,                       XK_y,         setlayout,      {.v = &layouts[2]} }, /* spiral */
	{ MODKEY|ShiftMask,             XK_y,         setlayout,      {.v = &layouts[3]} }, /* dwindle */
	{ MODKEY,                       XK_u,         setlayout,      {.v = &layouts[4]} }, /* deck */
	{ MODKEY|ShiftMask,             XK_u,         setlayout,      {.v = &layouts[5]} }, /* monocle */
	{ MODKEY,                       XK_i,         setlayout,      {.v = &layouts[6]} }, /* centeredmaster */
	{ MODKEY|ShiftMask,             XK_i,         setlayout,      {.v = &layouts[7]} }, /* centeredfloatingmaster */
	{ MODKEY|ShiftMask,             XK_f,         setlayout,      {.v = &layouts[8]} }, /* float */
	{ MODKEY,                       XK_0,         view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,         tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,     focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period,    focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,     tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period,    tagmon,         {.i = +1 } },
	{ MODKEY,                       XK_grave,     togglescratch,  {.ui = 0 } },
	{ MODKEY|ShiftMask,             XK_q,         quit,           {0} },
	TAGKEYS(                        XK_1,                         0)
	TAGKEYS(                        XK_2,                         1)
	TAGKEYS(                        XK_3,                         2)
	TAGKEYS(                        XK_4,                         3)
	TAGKEYS(                        XK_5,                         4)
	TAGKEYS(                        XK_6,                         5)
	TAGKEYS(                        XK_7,                         6)
	TAGKEYS(                        XK_8,                         7)
	TAGKEYS(                        XK_9,                         8)
	{ ALTKEY,                       XK_v,         spawn,          SHCMD("$TERMINAL -e $EDITOR") },
	{ ALTKEY,                       XK_f,         spawn,          SHCMD("$TERMINAL -e nnn -Rr") },
	{ ALTKEY,                       XK_m,         spawn,          SHCMD("$TERMINAL -e ncmpcpp") },
	{ ALTKEY,                       XK_e,         spawn,          SHCMD("$TERMINAL -e neomutt") },
	{ ALTKEY|ShiftMask,             XK_e,         spawn,          SHCMD("mw -Y; $TERMINAL -e neomutt") },
	{ ALTKEY,                       XK_b,         spawn,          SHCMD("$BROWSER") },
	{ ALTKEY,                       XK_p,         spawn,          SHCMD("mpv $(xclip -o)") },
	{ ALTKEY|ShiftMask,             XK_l,         spawn,          SHCMD("slock") },
	{ ALTKEY,                       XK_equal,     spawn,          SHCMD("volumectl up") },
	{ ALTKEY,                       XK_minus,     spawn,          SHCMD("volumectl down") },
	{ ALTKEY|ShiftMask,             XK_BackSpace, spawn,          SHCMD("volumectl toggle") },
	{ MODKEY|ShiftMask,             XK_BackSpace, spawn,          SHCMD("dmenushutdown") },
	{ 0,                            XK_Print,     spawn,          SHCMD("dmenuscreenshot") },
	{ MODKEY|ShiftMask,             XK_p,         spawn,          SHCMD("passmenu") },
	{ ALTKEY|ShiftMask,             XK_period,    spawn,          SHCMD("mpc -q next") },
	{ ALTKEY|ShiftMask,             XK_comma,     spawn,          SHCMD("mpc -q prev") },
	{ ALTKEY|ShiftMask,             XK_r,         spawn,          SHCMD("mpc -q repeat") },
	{ ALTKEY|ShiftMask,             XK_z,         spawn,          SHCMD("mpc -q random") },
	{ ALTKEY|ShiftMask,             XK_s,         spawn,          SHCMD("mpc -q single") },
	{ ALTKEY|ShiftMask,             XK_a,         spawn,          SHCMD("mpc -q seek 0") },
	{ ALTKEY|ShiftMask,             XK_space,     spawn,          SHCMD("mpc -q toggle") },
	{ ALTKEY|ShiftMask,             XK_equal,     spawn,          SHCMD("mpc -q volume +5") },
	{ ALTKEY|ShiftMask,             XK_minus,     spawn,          SHCMD("mpc -q volume -5") },
	{ ALTKEY|ShiftMask,             XK_p,         spawn,          SHCMD("notify-send -a Music Playing \"$(mpc current)\"") },
	{ ALTKEY,                       XK_BackSpace, spawn,          SHCMD("dunstctl set-paused toggle") },
	{ ALTKEY,                       XK_backslash, spawn,          SHCMD("dunstctl history-pop") },
	{ ALTKEY|ShiftMask,             XK_backslash, spawn,          SHCMD("dunstctl close-all") },
	{ 0,                            XK_F4,        spawn,          SHCMD("keymapswitch us ar") },
	{ 0,                            XK_F5,        spawn,          SHCMD("dmenuhandler") },


	/*
	{ MODKEY,                       XK_space,     setlayout,      {0} },
	{ MODKEY|ControlMask,           XK_space,     focusmaster,    {0} },

	{ MODKEY|Mod1Mask,              XK_u,         incrgaps,       {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_u,         incrgaps,       {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_i,         incrigaps,      {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_i,         incrigaps,      {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_o,         incrogaps,      {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_o,         incrogaps,      {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_6,         incrihgaps,     {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_6,         incrihgaps,     {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_7,         incrivgaps,     {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_7,         incrivgaps,     {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_8,         incrohgaps,     {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_8,         incrohgaps,     {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_9,         incrovgaps,     {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_9,         incrovgaps,     {.i = -1 } },
	*/
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	//{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	//{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        ShiftMask,      Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        defaultgaps,    {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkClientWin,         MODKEY,         Button4,        incrgaps,       {.i = +1} },
	{ ClkClientWin,         MODKEY,         Button5,        incrgaps,       {.i = -1} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
