#ifndef _TEXT_FORMAT_HPP
#define _TEXT_FORMAT_HPP

/// \file TextFormat.hpp
///
/// \brief Define format to print on the terminal
///
/// See https://www.growingwiththeweb.com/2015/05/colours-in-gnome-terminal.html

namespace SUNphi
{
  /// First part of the string which change color
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
  
  
  /// First part of the string which change style
  [[ maybe_unused ]]
  static constexpr char TEXT_CHANGE_STYLE_HEAD[]=
    "\x1b[";
  
  /// Last part of the string whcih change style
  [[ maybe_unused ]]
  static constexpr char TEXT_CHANGE_STYLE_TAIL[]=
    "m";
  
  /// Identifier of the various supported styles
  enum class TextStyle : char {RESET='0',
			       BOLD='1',
			       DIM='2',
			       ITALIC='3',
			       UNDERLINE='4',
			       BLINK='5',
			       REVERSE='7',
			       HIDDEN='8',
			       STRIKETHROUGH='9'};
}

#endif
