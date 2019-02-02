#ifndef _TEXTCOLORS_HPP
#define _TEXTCOLORS_HPP

/// \file TextColors.hpp
///
/// \brief Define colors to print on the terminal
///
/// See https://www.growingwiththeweb.com/2015/05/colours-in-gnome-terminal.html

namespace SUNphi
{
  /// First part of the string whcih change color
  [[ maybe_unused ]]
  static constexpr char TEXT_CHANGE_COLOR_HEAD[]=
    "\x1b[3";
  
  /// Last part of the string whcih change color
  [[ maybe_unused ]]
  static constexpr char TEXT_CHANGE_COLOR_TAIL[]=
    "m";
  
  /// Identifier of the various supported colors
  enum class TextColor : char {BLACK='0',
			       RED='1',
			       GREEN='2',
			       BROWN='3',
			       BLUE='4',
			       PURPLE='5',
			       CYAN='6',
			       GREY='7',
			       DEFAULT='9'};
  
  
  #undef DEFINE_TEXT_COLOR
}

#endif
