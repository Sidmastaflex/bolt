#
#  .profile -- Login shell initialization file, used by sh/bash.
#
#   Defaults for UCR Dept. of Computer Science
#   by Will Johnston (will@cs.ucr.edu)  25 Mar 97
#   And modified by CSE Technical staff since then.

#
# Setup environment variables. (Only necessary for login shells
# because environment variables are passed on to child processes.)
#

# Setup execution search path.
PATH=~/bin:/usr/local/bin:/usr/bin:/bin:/usr/ucb:/sbin:/usr/sbin:/usr/local/sbin:/usr/bin/X11:/usr/X11R6/bin:/usr/local/X11:/usr/local/bin/X11:/usr/openwin/bin:.

# Setup default editor.
EDITOR=emacs
VISUAL=emacs

# Setup news server.
NNTPSERVER=news.cs.ucr.edu

# Hardcode timezone.
TZ=US/Pacific

# Convert these to environment variables.
export PATH EDITOR VISUAL NNTPSERVER TZ

#
# Setup terminal.  (Also only done for login shells.)
#

export TERM=xterm-color

#
# If bash is the shell, source the .bashrc file, as well.
#
if [ $?BASH ] ; then
  source $HOME/.bashrc
fi




