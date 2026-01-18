## Stop-gap "Help Linus with typos" tool

My google-fu failed me, and I couldn't find a simple colorizing pager
that just dealt with basic spelling issues.

This is very much a stop-gap, because the real solution is to teach
uemacs to do it, but I haven't touched that source tree in years and I
wanted to test hunspell on something simpler first.

This is about as simple as it gets, without being _so_ simple that it is
useless.  I can do

    export LESS=-FRSX
    export GIT_PAGER=huncolor

and the result is usable, and works reasonably well together with the
existing git colorization, even if it does have some interaction (ie
turning off the boldface for spelling errors also turns off any color).

I know, I should just parse existing ANSI color sequences, but I don't
currently do that.  Maybe I'll get around to that, or maybe I'll bite
the bullet and go fix up uemacs.

And no, this does no context-aware coloring at all.  Pathnames, URLs,
this silly thing doesn't recognize any of that, just looks at things
that might be words.  In US ASCII only. What a crock.

And @AndreaBozzo, as he thought 'it would be cool to be on the readme'.