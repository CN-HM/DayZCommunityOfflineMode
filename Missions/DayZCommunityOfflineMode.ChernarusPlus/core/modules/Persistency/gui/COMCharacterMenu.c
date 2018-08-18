class COMCharacterMenu extends UIScriptedMenu
{
	protected ref PersistencyModule 		m_oPersistencyModule;

	protected Widget						m_CharacterRotationFrame;
	protected ButtonWidget					m_Apply;
	protected ButtonWidget					m_NewCharacter;
	protected ButtonWidget					m_Cancel;

	protected ButtonWidget					m_PrevCharacter;
	protected TextWidget					m_CharacterText;
	protected ButtonWidget					m_NextCharacter;

	protected EditBoxWidget					m_PlayerName;
	protected TextWidget					m_ActionTitle;
	protected TextWidget					m_Version;

	protected ref OptionSelectorMultistate	m_SaveSelector;

	protected ref OptionSelectorMultistate	m_GenderSelector;
	protected ref OptionSelectorMultistate	m_SkinSelector;
	protected ref OptionSelectorMultistate	m_TopSelector;
	protected ref OptionSelectorMultistate	m_BottomSelector;
	protected ref OptionSelectorMultistate	m_ShoesSelector;

    protected bool                          m_IsLoadingSave;
	
	protected ref map< string, ref TStringAdvanceArray > m_Saves = new map< string, ref TStringAdvanceArray >;

	protected int							m_Character;
    protected int 							m_Save;
	protected bool							m_NoSaves;

    void COMCharacterMenu( ref PersistencyModule oPersistencyModule )
	{
		m_oPersistencyModule = oPersistencyModule;

        m_IsLoadingSave = true;
		m_NoSaves = true;

		SetCharacterList();
	}

    void ~COMCharacterMenu()
    {
		GetGame().GetUpdateQueue(CALL_CATEGORY_SYSTEM).Remove(this.UpdateInterval);
    }
	
	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets( "missions\\DayZCommunityOfflineMode.ChernarusPlus\\core\\modules\\Persistency\\gui\\layouts\\COMCharacterCreation.layout" );

		m_CharacterRotationFrame            = layoutRoot.FindAnyWidget( "character_rotation_frame" );

        m_Apply								= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "apply" ) );
		m_NewCharacter						= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "new_character" ) );
		m_Cancel							= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "cancel" ) );

		m_PrevCharacter						= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "prev_character" ) );
		m_CharacterText						= TextWidget.Cast( layoutRoot.FindAnyWidget( "character_text" ) );
		m_NextCharacter						= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "next_character" ) );
        
        m_PlayerName						= EditBoxWidget.Cast( layoutRoot.FindAnyWidget( "general_name_setting_text" ) );
		m_ActionTitle						= TextWidget.Cast( layoutRoot.FindAnyWidget( "CharacterCreationTextWidget" ) );
		m_Version							= TextWidget.Cast( layoutRoot.FindAnyWidget( "version" ) );
		
        string version;
		GetGame().GetVersion( version );
		if( version != "" )
			m_Version.SetText( "#main_menu_version" + " " + version );
		else
			m_Version.Show( false );


		m_CharacterText.SetText( GetCharacter() );
		TStringAdvanceArray saves = GetSavesForCharacter( GetCharacter() );

		if ( saves.Count() == 0 ) {
			m_NoSaves = true;

			ref TStringAdvanceArray ts = new TStringAdvanceArray;
			ts.Insert("temp");
			m_SaveSelector = new OptionSelectorMultistate( layoutRoot.FindAnyWidget( "character_save_setting_option" ), 0, null, false, ts );
		} else {
			m_NoSaves = false;
			m_SaveSelector = new OptionSelectorMultistate( layoutRoot.FindAnyWidget( "character_save_setting_option" ), 0, null, false, saves );
		}

		m_GenderSelector = new OptionSelectorMultistate( layoutRoot.FindAnyWidget( "character_gender_setting_option" ), 0, null, false, m_oPersistencyModule.GetScene().m_CharGenderList );

		if ( m_oPersistencyModule.GetScene().GetCharacterGender() == CharGender.FEMALE )
		{
			m_GenderSelector.SetValue( "Female" );
			m_SkinSelector	= new OptionSelectorMultistate( layoutRoot.FindAnyWidget( "character_head_setting_option" ), 0, null, false, m_oPersistencyModule.GetScene().m_CharPersonalityFemaleList );
		}
		else
		{
			m_GenderSelector.SetValue( "Male" );
			m_SkinSelector	= new OptionSelectorMultistate( layoutRoot.FindAnyWidget( "character_head_setting_option" ), 0, null, false, m_oPersistencyModule.GetScene().m_CharPersonalityMaleList );
		}

		m_TopSelector						= new OptionSelectorMultistate( layoutRoot.FindAnyWidget( "character_top_setting_option" ), 0, null, false, m_oPersistencyModule.GetScene().m_CharShirtList );
		m_BottomSelector					= new OptionSelectorMultistate( layoutRoot.FindAnyWidget( "character_bottom_setting_option" ), 0, null, false, m_oPersistencyModule.GetScene().m_CharPantsList );
		m_ShoesSelector						= new OptionSelectorMultistate( layoutRoot.FindAnyWidget( "character_shoes_setting_option" ), 0, null, false, m_oPersistencyModule.GetScene().m_CharShoesList );

		m_SaveSelector.m_OptionChanged.Insert( SaveChanged );
		m_GenderSelector.m_OptionChanged.Insert( GenderChanged );
		m_SkinSelector.m_OptionChanged.Insert( SkinChanged );
		m_TopSelector.m_OptionChanged.Insert( TopChanged );
		m_BottomSelector.m_OptionChanged.Insert( BottomChanged );
		m_ShoesSelector.m_OptionChanged.Insert( ShoesChanged );

		return layoutRoot;
	}

    bool IsLoadingSave()
    {
        return m_IsLoadingSave;
    }

	void SetOptions()
	{
		if (m_IsLoadingSave)
		{	
			m_ActionTitle.SetText( "SELECT SAVE" );
			m_NewCharacter.SetText( "NEW CHARACTER" );

			layoutRoot.FindAnyWidget( "prev_character" ).Show( true );
			layoutRoot.FindAnyWidget( "next_character" ).Show( true );

			layoutRoot.FindAnyWidget( "character_content" ).Show( false );
			layoutRoot.FindAnyWidget( "character_save" ).Show( true );

			if (m_NoSaves)
			{
				layoutRoot.FindAnyWidget( "character_save_setting" ).Show( false );
				layoutRoot.FindAnyWidget( "character_save_setting_disable" ).Show( true );
			} else 
			{
				layoutRoot.FindAnyWidget( "character_save_setting" ).Show( true );
				layoutRoot.FindAnyWidget( "character_save_setting_disable" ).Show( false );
			}
		} else 
		{
			m_ActionTitle.SetText( "CREATE NEW CHARACTER" );
			m_NewCharacter.SetText( "SELECT SAVE" );

			m_CharacterText.SetText( "CHARACTER DETAILS" );

			layoutRoot.FindAnyWidget( "prev_character" ).Show( false );
			layoutRoot.FindAnyWidget( "next_character" ).Show( false );

			layoutRoot.FindAnyWidget( "character_content" ).Show( true );
			layoutRoot.FindAnyWidget( "character_save" ).Show( false );
		}
	}
    
    override bool UseMouse()
	{
		return true;
	}

	override bool UseKeyboard()
	{
		return true;
	}

	void Cancel()
	{
        m_oPersistencyModule.LoadLast();

		GetGame().GetUIManager().CloseMenu( MENU_INGAME );
	}

	void Apply()
	{
		if ( m_IsLoadingSave )
		{
        	m_oPersistencyModule.LoadPlayer( GetCharacter(), GetSave() );
		} else {
			string characterName = m_PlayerName.GetText();

			bool canCreate = true;

			if ( characterName == "" )
			{
				GetGame().GetUIManager().ShowDialog("FAILURE", "You didn't give a character name!", 0, DBT_OK, DBB_OK, DMT_WARNING, NULL);
				canCreate = false;
			}

			if ( canCreate && FileExist(BASE_PLAYER_SAVE_DIR + "\\" + characterName) )
			{
				GetGame().GetUIManager().ShowDialog("FAILURE", "That character already exists!", 0, DBT_OK, DBB_OK, DMT_WARNING, NULL);
				canCreate = false;
			}

			if ( canCreate )
			{
        		m_oPersistencyModule.CreatePlayer( characterName, m_oPersistencyModule.GetScene().GetPlayerUnit(), "latest" );
			}
		}

		GetGame().GetUIManager().CloseMenu( MENU_INGAME );
	}

	void NewCharacter()
	{
		m_IsLoadingSave = !m_IsLoadingSave;
		
		SetCharacter(m_Character);
	}

	void PreviousCharacter()
	{
		m_Character--;
		SetCharacter(m_Character);
	}

	void NextCharacter()
	{
		m_Character++;
		SetCharacter(m_Character);
	}

	void SetCharacter( int index )
	{
		if ( index < 0 ) index = m_Saves.Count() - 1;
		if ( index >= m_Saves.Count() - 1 ) index = 0;

		m_CharacterText.SetText( GetCharacter() );

		TStringAdvanceArray saves = GetSavesForCharacter( GetCharacter() );

		if ( saves.Count() == 0 ) {
			m_NoSaves = true;
		} else {
			m_NoSaves = false;
			m_SaveSelector = new OptionSelectorMultistate( layoutRoot.FindAnyWidget( "character_save_setting_option" ), 0, null, false, saves );
		}
	
		m_Character = index;
	}
    
    void UpdateInterval()
	{
		MissionGameplay.Cast(GetGame().GetMission()).Continue();
		GetGame().GetUIManager().CloseMenu( MENU_INGAME );

        m_oPersistencyModule.GetScene().Update();

		SetOptions();
	}
    
    override void OnShow()
	{
		super.OnShow();

        GetGame().GetInput().ChangeGameFocus( 1, INPUT_DEVICE_MOUSE );
        GetGame().GetUIManager().ShowUICursor( true );

        GetGame().GetMission().GetHud().Show(false);

		GetGame().GetUpdateQueue(CALL_CATEGORY_SYSTEM).Insert(this.UpdateInterval);
	}
    
    override void OnHide()
	{
        GetGame().GetUIManager().ShowUICursor( false );
        GetGame().GetInput().ResetGameFocus( INPUT_DEVICE_MOUSE );
        
		GetGame().GetUpdateQueue(CALL_CATEGORY_SYSTEM).Remove(this.UpdateInterval);

		GetGame().GetUIManager().CloseMenu( MENU_INGAME );

        if ( !m_IsLoadingSave )
        {
            m_oPersistencyModule.LoadLast();
        }

		super.OnHide();
	}
    
    override bool OnKeyPress( Widget w, int x, int y, int key )
	{
        Print("A key of stuffs was pressed!");

        if ( key == KeyCode.KC_ESCAPE )
        {
            Print("Called the escape a rino");

            m_oPersistencyModule.LoadLast();

		    GetGame().GetUIManager().CloseMenu( MENU_INGAME );

			return true;
        }

		return false;
	}

	override bool OnClick( Widget w, int x, int y, int button )
	{
		if( w == m_Apply )
		{
			Apply();
			return true;
		}
		else if ( w == m_NewCharacter )
		{
			NewCharacter();
			return true;
		}
		else if ( w == m_Cancel )
		{
			Cancel();
			return true;
		}
		else if ( w == m_NextCharacter )
		{
			NextCharacter();
			return true;
		}
		else if ( w == m_PrevCharacter )
		{
			PreviousCharacter();
			return true;
		}
		return false;
	}
    
    override bool OnMouseButtonDown( Widget w, int x, int y, int button )
	{
		if ( w == m_CharacterRotationFrame )
		{
			if ( m_oPersistencyModule.GetScene() )
				m_oPersistencyModule.GetScene().CharacterRotationStart();
			return true;
		}
		return false;
	}
	
	override bool OnMouseButtonUp( Widget w, int x, int y, int button )
	{
		if ( m_oPersistencyModule.GetScene() )
			m_oPersistencyModule.GetScene().CharacterRotationStop();
		return false;
	}

	override bool OnMouseEnter( Widget w, int x, int y )
	{
		if( IsFocusable( w ) )
		{
			ColorRed( w );
			return true;
		}
		return false;
	}
	
	override bool OnMouseLeave( Widget w, Widget enterW, int x, int y )
	{
		if( IsFocusable( w ) )
		{
			ColorWhite( w, enterW );
			return true;
		}
		return false;
	}
	
	override bool OnFocus( Widget w, int x, int y )
	{
		if( IsFocusable( w ) )
		{
			ColorRed( w );
			return true;
		}
		return false;
	}
	
	override bool OnFocusLost( Widget w, int x, int y )
	{
		if( IsFocusable( w ) )
		{
			ColorWhite( w, null );
			return true;
		}
		return false;
	}
	
	bool IsFocusable( Widget w )
	{
		return ( w == m_Apply || w == m_NewCharacter || w == m_Cancel || w == m_PrevCharacter || w == m_NextCharacter );
	}

	//Coloring functions (Until WidgetStyles are useful)
	void ColorRed( Widget w )
	{
		SetFocus( w );

		TextWidget text		= TextWidget.Cast(w.FindWidget( w.GetName() + "_text" ) );
		TextWidget text2	= TextWidget.Cast(w.FindWidget( w.GetName() + "_text_1" ) );
		ImageWidget image	= ImageWidget.Cast( w.FindWidget( w.GetName() + "_image" ) );
		
		if( text )
		{
			text.SetColor( ARGB( 255, 255, 0, 0 ) );
		}
		
		if( text2 )
		{
			text2.SetColor( ARGB( 255, 255, 0, 0 ) );
		}
		
		if( image )
		{
			image.SetColor( ARGB( 255, 255, 0, 0 ) );
		}
	}
	
	void ColorWhite( Widget w, Widget enterW )
	{
		#ifdef PLATFORM_WINDOWS
		SetFocus( null );
		#endif
		
		TextWidget text		= TextWidget.Cast(w.FindWidget( w.GetName() + "_text" ) );
		TextWidget text2	= TextWidget.Cast(w.FindWidget( w.GetName() + "_text_1" ) );
		ImageWidget image	= ImageWidget.Cast( w.FindWidget( w.GetName() + "_image" ) );
		
		if( text )
		{
			text.SetColor( ARGB( 255, 255, 255, 255 ) );
		}
		
		if( text2 )
		{
			text2.SetColor( ARGB( 255, 255, 255, 255 ) );
		}
		
		if( image )
		{
			image.SetColor( ARGB( 255, 255, 255, 255 ) );
		}
	}

	TStringAdvanceArray GetSavesForCharacter(string sCharacter)
	{
		ref TStringAdvanceArray oSaves = m_Saves.Get(sCharacter);
		string sName = "";
		FileAttr oFileAttr = FileAttr.INVALID;
		FindFileHandle oFileHandle = FindFile(BASE_PLAYER_SAVE_DIR + "\\" + sCharacter + "\\*.json", sName, oFileAttr, FindFileFlags.ALL);

		if (sName != "")
		{
			oSaves.Clear();

			if (sName != ".json")
			{
				oSaves.Insert(sName.Substring(0, sName.Length() - 5));
			}

			while (FindNextFile(oFileHandle, sName, oFileAttr))
			{
				if (sName != "" && sName != ".json")
				{
					oSaves.Insert(sName.Substring(0, sName.Length() - 5));
				}
			}
		}

		return oSaves;
	}

	void SetCharacterList()
	{
		string sName = "";
		FileAttr oFileAttr = FileAttr.INVALID;
		FindFileHandle oFileHandle = FindFile(BASE_PLAYER_SAVE_DIR + "\\*", sName, oFileAttr, FindFileFlags.ALL);

		Print("File Attributes: INVALID " + FileAttr.INVALID + " DIRECTORY " + FileAttr.DIRECTORY + " HIDDEN " + FileAttr.HIDDEN + " READONLY " + FileAttr.READONLY);

		Print("Character: " + sName + ", File Attr: " + oFileAttr);

		if (sName != "")
		{
			if (sName != GetCharacter())
			{
				m_Saves.Set(sName, new TStringAdvanceArray);
			}

			while (FindNextFile(oFileHandle, sName, oFileAttr))
			{
				if (sName != GetCharacter())
				{
					Print("Character: " + sName + ", File Attr: " + oFileAttr);
					m_Saves.Set(sName, new TStringAdvanceArray);
				}
			}
		}
	}

	string GetCharacter()
	{
		return m_Saves.GetKey( m_Character );
	}

	string GetSave()
	{
		TStringAdvanceArray saves = m_Saves.Get( GetCharacter() );
		if ( saves == NULL )
		{
			return "";
		}

		return saves.Get( m_Save );
	}

	void SaveChanged()
	{
		m_Save = m_SaveSelector.GetValue();
	}

	void SetGender( string gender )
	{
		switch ( gender )
		{
			case "Female":
			{
				m_oPersistencyModule.GetScene().SetCharacterGender( CharGender.FEMALE );
				m_SkinSelector.LoadNewValues( m_oPersistencyModule.GetScene().m_CharPersonalityFemaleList, 0 );
				m_SkinSelector.SetRandomValue();
				break;
			}
			case "Male":
			{
				m_oPersistencyModule.GetScene().SetCharacterGender( CharGender.MALE );
				m_SkinSelector.LoadNewValues( m_oPersistencyModule.GetScene().m_CharPersonalityMaleList, 0 );
				m_SkinSelector.SetRandomValue();
				break;
			}
		}
	}

	void GenderChanged()
	{
		SetGender( m_GenderSelector.GetStringValue() );
	}
	
	void SkinChanged()
	{
		m_oPersistencyModule.GetScene().CreateNewCharacter( m_SkinSelector.GetStringValue() );
		
		layoutRoot.FindAnyWidget( "character_root" ).Show( g_Game.IsNewCharacter() );
		
		TopChanged();
		BottomChanged();
		ShoesChanged();
		
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater( m_oPersistencyModule.GetScene().ResetIntroCamera, 100 );
	}
	
	void TopChanged()
	{
		m_oPersistencyModule.GetScene().SetAttachment( m_TopSelector.GetStringValue(), InventorySlots.BODY );
	}
	
	void BottomChanged()
	{
		m_oPersistencyModule.GetScene().SetAttachment( m_BottomSelector.GetStringValue(), InventorySlots.LEGS );
	}
	
	void ShoesChanged()
	{
		m_oPersistencyModule.GetScene().SetAttachment( m_ShoesSelector.GetStringValue(), InventorySlots.FEET );
	}
}