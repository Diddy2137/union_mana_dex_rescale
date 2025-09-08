META
{
	Parser = Menu;
	After = zUnionMenu.d;
	Namespace = DamageScaling;
};

const int HEADLINE_Y = 550;
const int DY = 550;
const int TEXT_X = 450;
const int TEXT_Y = 1650;
const int CHOICE_X = 5500;
const int CHOICE_DIMX = 2200;
const int SLIDER_Y = 1500;

prototype C_MENU_ITEM_TEXT(C_MENU_ITEM)
{
	C_MENU_ITEM_DEF();
	fontname = MENU_FONT_SMALL;
	flags = flags | IT_EFFECTS_NEXT;
	posx = TEXT_X;
	posy = TEXT_Y;
	onselaction[0] = SEL_ACTION_UNDEF;
};

prototype C_MENU_ITEM_CHOICE(C_MENU_ITEM)
{
	C_MENU_ITEM_DEF();
	backpic = MENU_CHOICE_BACK_PIC;
	type = MENU_ITEM_CHOICEBOX;
	fontname = MENU_FONT_SMALL;
	flags = flags & ~IT_SELECTABLE;
	flags = flags | IT_TXT_CENTER;
	posx = CHOICE_X;
	posy = TEXT_Y;
	dimx = CHOICE_DIMX;
	onchgsetoptionsection = "DamageScaling";

	text[0] = Str_GetLocalizedString(
		"НЕТ|Да", 
		"Nein|Ja", 
		"off|on", 
		"Nie|Tak"
	);
};

prototype C_MENU_ITEM_SLIDER(C_MENU_ITEM)
{
	C_MENU_ITEM_DEF();
	backpic = MENU_SLIDER_BACK_PIC;
	type = MENU_ITEM_SLIDER;
	userstring[0] = MENU_SLIDER_POS_PIC;
	userfloat[1] = 9.0; // 0..8 (9 positions), override per item if needed
	posx = CHOICE_X;
	posy = SLIDER_Y;
	dimx = CHOICE_DIMX;
	flags = flags & ~IT_SELECTABLE;
	onchgsetoptionsection = "DamageScaling";
};

instance :MenuItem_Union_Auto_DamageScaling(C_MENU_ITEM_UNION_DEF)
{
	text[0] = "Damage Scaling";
	text[1] = Str_GetLocalizedString(
		"Настройки масштабирования урона",
		"Skalierungsfaktoren konfigurieren",
		"Configure scaling factors",
		"Skonfiguruj współczynniki skalowania"
	);

	onselaction[0] = SEL_ACTION_STARTMENU;
	onselaction_s[0] = "DamageScaling:Menu_Options";
};

instance Menu_Options(C_MENU_DEF)
{
	backpic = "MENU_INGAME.TGA";
	items[0] = "";
	flags = flags | MENU_SHOW_INFO;
	Menu_SearchItems("DamageScaling:MenuItem_*");
};

instance MenuItem_HeadLine(C_MENU_ITEM_TEXT)
{
	fontname = MENU_FONT_DEFAULT;
	flags = flags & ~IT_SELECTABLE;
	flags = flags | IT_TXT_CENTER;
	posx = 0;
	posy = HEADLINE_Y;
	text[0] = "Damage Scaling";
};

instance MenuItem_DexScale_Text(C_MENU_ITEM_TEXT)
{
	posy += 0 * DY;
	text[0] = Str_GetLocalizedString(
		"Шкала ловкости",
		"Geschicklichkeits Skala",
		"Dexterity Scale",
		"Skalowanie zręczności"
	);
};
instance MenuItem_DexScale_Slider(C_MENU_ITEM_SLIDER)
{
	posy += 0 * DY;
	onchgsetoption = "DexScale";
	userfloat[0] = 8.0;
};
instance MenuItem_ManaScale_Text(C_MENU_ITEM_TEXT)
{
	posy += 1 * DY;
	text[0] = Str_GetLocalizedString(
		"Шкала маны",
		"Mana Scala",
		"Mana Scale",
		"Skalowanie Many"
	);
};
instance MenuItem_ManaScale_Slider(C_MENU_ITEM_SLIDER)
{
	posy += 1 * DY;
	onchgsetoption = "ManaScale";
	userfloat[0] = 8.0;
};
instance MenuItem_DebugOutput_Text(C_MENU_ITEM_TEXT)
{
	posy += 2 * DY;
	text[0] = Str_GetLocalizedString(
		"Отладочный вывод",
		"Debug Output",
		"Debug Output",
		"Debug Output"
	);
};
instance MenuItem_DebugOutput_Choice(C_MENU_ITEM_CHOICE)
{
	posy += 2 * DY;
	onchgsetoption = "DebugOutput";
};
instance MenuItem_SneakAmbush_Text(C_MENU_ITEM_TEXT)
{
	posy += 3 * DY;
	text[0] = Str_GetLocalizedString(
		"SneakAmbush",
		"SneakAmbush",
		"SneakAmbush",
		"SneakAmbush"
	);
};
instance MenuItem_SneakAmbush_Choice(C_MENU_ITEM_CHOICE)
{
	posy += 3 * DY;
	onchgsetoption = "SneakAmbush";
};
instance MenuItem_DontKill_Text(C_MENU_ITEM_TEXT)
{
	posy += 4 * DY;
	text[0] = Str_GetLocalizedString(
		"DontKill",
		"DontKill",
		"DontKill",
		"DontKill"
	);
};
instance MenuItem_Nonlethal_Choice(C_MENU_ITEM_CHOICE)
{
	posy += 4 * DY;
	onchgsetoption = "DontKill";
};

instance MenuItem_Back(C_MENU_ITEM_DEF)
{
	Union_MenuItem_Back();
};
