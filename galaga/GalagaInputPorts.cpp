#include "GalagaInputPorts.h"
#include "GalagaMachine.h"

namespace Galaga
{
uint8_t IN0LPort::Read()
{
    // PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 )
    // PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
    // PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_START1 )
    // PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START2 )
    return (uint8_t)((Button1 ? 0 : 0x01) | 0x02 | (Start1Player ? 0 : 0x04) |
                     (Start2Player ? 0 : 0x08));
}

uint8_t IN0HPort::Read()
{
    // PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
    // PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
    // PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE1 )
    // PORT_SERVICE( 0x08, IP_ACTIVE_LOW )
    return (uint8_t)((InsertCoin ? 0 : 0x01) | 0x02 | 0x04 | 0x08);
}

uint8_t IN1LPort::Read()
{
    // PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNUSED )
    // PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_2WAY
    // PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )
    // PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_2WAY
    return (uint8_t)(0x01 | (MoveRight ? 0 : 0x02) | 0x04 |
                     (MoveLeft ? 0 : 0x08));
}

uint8_t IN1HPort::Read()
{
    // PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNUSED )
    // PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_2WAY
    // PORT_COCKTAIL PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED ) PORT_BIT( 0x08,
    // IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_2WAY PORT_COCKTAIL
    return 0x0f;
}

uint8_t DSWAPort::Read()
{
    // PORT_DIPNAME( 0x01, 0x01, "2 Credits Game" )
    // PORT_DIPLOCATION("SW1:1") PORT_DIPSETTING(    0x00, "1 Player" )
    // PORT_DIPSETTING(    0x01, "2 Players" )
    int val = galaga.twoPlayersGame ? 0x01 : 0x00;

    // PORT_DIPNAME( 0x06, 0x06, DEF_STR( Difficulty ) )
    // PORT_DIPLOCATION("SW1:2,3") PORT_DIPSETTING(    0x06, DEF_STR( Easy ) )
    // PORT_DIPSETTING(    0x00, DEF_STR( Medium ) )
    // PORT_DIPSETTING(    0x02, DEF_STR( Hard ) )
    // PORT_DIPSETTING(    0x04, DEF_STR( Hardest ) )
    val |= ((int)galaga.difficulty * 2);

    // PORT_DIPNAME( 0x08, 0x00, DEF_STR( Demo_Sounds ) )
    // PORT_DIPLOCATION("SW1:4") PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
    // PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    val |= (galaga.demoSound ? 0x00 : 0x08);

    // PORT_DIPNAME( 0x10, 0x10, "Freeze" )
    // PORT_DIPLOCATION("SW1:5") PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
    // PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    val |= (galaga.freeze ? 0x00 : 0x10);

    // PORT_DIPNAME( 0x20, 0x20, "Rack Test" )
    // PORT_DIPLOCATION("SW1:6") PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
    // PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    val |= (galaga.rackTest ? 0x00 : 0x20);

    // PORT_DIPUNUSED_DIPLOC( 0x40, 0x40, "SW1:7" )		/* Listed as "Unused"
    // */
    val |= 0x40;

    // PORT_DIPNAME( 0x80, 0x80, DEF_STR( Cabinet ) )
    // PORT_DIPLOCATION("SW1:8") PORT_DIPSETTING(    0x80, DEF_STR( Upright ) )
    // PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
    val |= (galaga.cabinetType == GalagaMachine::CabinetType::Upright ? 0x80
                                                                      : 0x00);

    return (uint8_t)val;
}

uint8_t DSWBPort::Read()
{
    // PORT_DIPNAME( 0x07, 0x07, DEF_STR( Coinage ) )
    // PORT_DIPLOCATION("SW2:1,2,3") PORT_DIPSETTING(    0x04, DEF_STR( 4C_1C ) )
    // PORT_DIPSETTING(    0x02, DEF_STR( 3C_1C ) )
    // PORT_DIPSETTING(    0x06, DEF_STR( 2C_1C ) )
    // PORT_DIPSETTING(    0x07, DEF_STR( 1C_1C ) )
    // PORT_DIPSETTING(    0x01, DEF_STR( 2C_3C ) )
    // PORT_DIPSETTING(    0x03, DEF_STR( 1C_2C ) )
    // PORT_DIPSETTING(    0x05, DEF_STR( 1C_3C ) )
    // PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) )
    int val = (int)galaga.coinage;

    // PORT_DIPNAME( 0xc0, 0x80, DEF_STR( Lives ) )
    // PORT_DIPLOCATION("SW2:7,8") PORT_DIPSETTING(    0x00, "2" )
    // PORT_DIPSETTING(    0x40, "4" )
    // PORT_DIPSETTING(    0x80, "3" )
    // PORT_DIPSETTING(    0xc0, "5" )
    val |= ((int)galaga.initialLives << 6);

    // PORT_DIPNAME( 0x38, 0x10, DEF_STR( Bonus_Life ) )
    // PORT_DIPLOCATION("SW2:4,5,6")
    // Began with 2, 3 or 4 fighters
    // PORT_DIPSETTING(    0x00, DEF_STR( None ) )
    // PORT_CONDITION("DSWB",0xc0,PORTCOND_NOTEQUALS,0xc0) PORT_DIPSETTING( 0x08,
    // "30K, 100K, Every 100K" )
    // PORT_CONDITION("DSWB",0xc0,PORTCOND_NOTEQUALS,0xc0) PORT_DIPSETTING( 0x10,
    // "20K, 70K, Every 70K" )
    // PORT_CONDITION("DSWB",0xc0,PORTCOND_NOTEQUALS,0xc0) PORT_DIPSETTING( 0x18,
    // "20K and 60K Only" ) PORT_CONDITION("DSWB",0xc0,PORTCOND_NOTEQUALS,0xc0)
    // PORT_DIPSETTING(    0x20, "20K, 60K, Every 60K" )
    // PORT_CONDITION("DSWB",0xc0,PORTCOND_NOTEQUALS,0xc0) PORT_DIPSETTING( 0x28,
    // "30K, 120K, Every 120K" )
    // PORT_CONDITION("DSWB",0xc0,PORTCOND_NOTEQUALS,0xc0) PORT_DIPSETTING( 0x30,
    // "20K, 80K, Every 80K" )
    // PORT_CONDITION("DSWB",0xc0,PORTCOND_NOTEQUALS,0xc0) PORT_DIPSETTING( 0x38,
    // "30K and 80K Only" ) PORT_CONDITION("DSWB",0xc0,PORTCOND_NOTEQUALS,0xc0)
    // Began with 5 fighters
    // PORT_DIPSETTING(    0x00, DEF_STR( None ) )
    // PORT_CONDITION("DSWB",0xc0,PORTCOND_EQUALS,0xc0) PORT_DIPSETTING(    0x08,
    // "30K and 100K Only" )     PORT_CONDITION("DSWB",0xc0,PORTCOND_EQUALS,0xc0)
    // PORT_DIPSETTING(    0x10, "30K, 120K, Every 120K" )
    // PORT_CONDITION("DSWB",0xc0,PORTCOND_EQUALS,0xc0) PORT_DIPSETTING(    0x18,
    // "30K and 150K Only" )     PORT_CONDITION("DSWB",0xc0,PORTCOND_EQUALS,0xc0)
    // PORT_DIPSETTING(    0x20, "30K, 100K, Every 100K" )
    // PORT_CONDITION("DSWB",0xc0,PORTCOND_EQUALS,0xc0) PORT_DIPSETTING(    0x28,
    // "30K and 120K Only" )     PORT_CONDITION("DSWB",0xc0,PORTCOND_EQUALS,0xc0)
    // PORT_DIPSETTING(    0x30, "30K, 150K, Every 150K" )
    // PORT_CONDITION("DSWB",0xc0,PORTCOND_EQUALS,0xc0) PORT_DIPSETTING(    0x38,
    // "30K Only" )              PORT_CONDITION("DSWB",0xc0,PORTCOND_EQUALS,0xc0)
    val |= ((int)galaga.bonusLives << 3);

    return (uint8_t)val;
}

}  // namespace Galaga