META
{
  Parser = Menu;
  After  = zUnionMenu.d;
};

const int DAMAGE_MENU_X       = 1000;
const int DAMAGE_MENU_Y_START = 1400;
const int DAMAGE_MENU_DY      = 550;

instance MENUITEM_UNION_AUTO_UNION_DAMAGE_SCALING(C_MENU_ITEM_UNION_DEF)
{
  text[0]          = "Damage Scaling";
  text[1]          = "Configure scaling factors";
  onselaction[0]   = SEL_ACTION_STARTMENU;
  onselaction_s[0] = "DAMAGE_SCALING_MENU";
};

instance DAMAGE_SCALING_MENU(C_MENU_DEF)
{
  backpic   = "MENU_INGAME.TGA";
  items[0]  = "DAMAGE_SCALING_TITLE";
  items[1]  = "DAMAGE_SCALING_DEX_TEXT";
  items[2]  = "DAMAGE_SCALING_DEX_SLIDER";
  items[3]  = "DAMAGE_SCALING_MANA_TEXT";
  items[4]  = "DAMAGE_SCALING_MANA_SLIDER";
  items[5]  = "DAMAGE_SCALING_DEBUG_TEXT";
  items[6]  = "DAMAGE_SCALING_DEBUG_CHOICE";
  items[19] = "UNION_MENUITEM_BACK";
  flags     = flags | MENU_SHOW_INFO;
};

instance DAMAGE_SCALING_TITLE(C_MENU_ITEM_DEF)
{
  type    = MENU_ITEM_TEXT;
  posx    = 0;
  posy    = 450;
  dimx    = 8100;
  flags   = flags & ~IT_SELECTABLE;
  flags   = flags | IT_TXT_CENTER;
  text[0] = "Damage Scaling Settings";
};

instance DAMAGE_SCALING_DEX_TEXT(C_MENU_ITEM_DEF)
{
  type    = MENU_ITEM_TEXT;
  posx    = DAMAGE_MENU_X;
  posy    = DAMAGE_MENU_Y_START + DAMAGE_MENU_DY * 1;
  text[0] = "Dexterity Scale";
};

instance DAMAGE_SCALING_DEX_SLIDER(C_MENU_ITEM_DEF)
{
  type                  = MENU_ITEM_CHOICEBOX;
  posx                  = 6000;
  posy                  = DAMAGE_MENU_Y_START + DAMAGE_MENU_DY * 1;
  dimx                  = 10000;
  dimy                  = 5000;
  onchgsetoption        = "DexScale";
  onchgsetoptionsection = "DamageScaling";
  text[0]               = "0|1|2|3|4|5|6|7|8"; // 9 positions
  userstring[0]         = "MENU_SLIDER_POS.TGA";
  userstring[1]         = "MENU_SLIDER_BACK.TGA";
};

instance DAMAGE_SCALING_MANA_TEXT(C_MENU_ITEM_DEF)
{
  type    = MENU_ITEM_TEXT;
  posx    = DAMAGE_MENU_X;
  posy    = DAMAGE_MENU_Y_START + DAMAGE_MENU_DY * 2;
  text[0] = "Mana Scale";
};

instance DAMAGE_SCALING_MANA_SLIDER(C_MENU_ITEM_DEF)
{
  type                  = MENU_ITEM_CHOICEBOX;
  posx                  = 6000;
  posy                  = DAMAGE_MENU_Y_START + DAMAGE_MENU_DY * 2;
  dimx                  = 10000;
  dimy                  = 5000;
  onchgsetoption        = "ManaScale";
  onchgsetoptionsection = "DamageScaling";
  text[0]               = "0|1|2|3|4|5|6|7|8";
  userstring[0]         = "MENU_SLIDER_POS.TGA";
  userstring[1]         = "MENU_SLIDER_BACK.TGA";
};

instance DAMAGE_SCALING_DEBUG_TEXT(C_MENU_ITEM_DEF)
{
  type    = MENU_ITEM_TEXT;
  posx    = DAMAGE_MENU_X;
  posy    = DAMAGE_MENU_Y_START + DAMAGE_MENU_DY * 3;
  text[0] = "Debug Output";
};

instance DAMAGE_SCALING_DEBUG_CHOICE(C_MENU_ITEM_DEF)
{
  type                  = MENU_ITEM_CHOICEBOX;
  posx                  = 6000;
  posy                  = DAMAGE_MENU_Y_START + DAMAGE_MENU_DY * 3;
  dimx                  = 1000;
  dimy                  = 350;
  onchgsetoption        = "DebugOutput";
  onchgsetoptionsection = "DamageScaling";
  text[0]               = "off|on";
};
