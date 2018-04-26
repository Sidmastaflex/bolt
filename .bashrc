#
#   .bashrc -- Per shell initialization for bash.
#
#   Defaults for UCR Dept. of Computer Science
#   by Will Johnston (will@cs.ucr.edu)  25 Mar 97
#   and modified by CSE Technical Staff since then.


#
# Setup emacs-style key bindings.
#
# bind -m emacs

#
# Remember the last hundred unique commands.
#
HISTSIZE=100
HISTCONTROL=ignoredups
HISTFILESIZE=100

#
# Setup nice prompts.
#
PS1="\w\n\u@\h \$ "
PS2="loop \$ "

#
# Protect against accidental overwrites with output redirection.
#
set noclobber

#
# Setup the command path.
#
PATH=~/bin:/usr/csshare/bin:/usr/csshare/sbin:/usr/bin/:/bin:/usr/local/bin:/usr/ucb:/sbin:/usr/sbin:/usr/local/sbin:/usr/bin/X11:/usr/X11R6/bin:/usr/local/X11:/usr/local/bin/X11:/usr/openwin/bin:.
export PATH

#
# Don't get lost in symlinks.
#
set nolinks

#
# Set default permissions for new files to be readable only by you.
#
umask 077

#
# Keep those pesky core dumps from filling up your account.
#
ulimit -c 0

#
# Setup some helpful aliases.
#

# nice ls format (flags)
alias ls="ls -F --color=tty -T 0"

# protect against accidental deletions/overwrites
alias cp="/bin/cp -i"
alias mv="/bin/mv -i"
alias rm="/bin/rm -i"
alias du='du -h'
# easy ways to logout - ALWAYS LOG OUT!
alias lo=exit
alias bye=exit

cd
unset MANPATH
