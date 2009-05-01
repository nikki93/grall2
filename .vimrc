version 6.0
if &cp | set nocp | endif
let s:cpo_save=&cpo
set cpo&vim
imap <S-Tab> 
inoremap <C-Tab> 	
noremap  h
noremap <NL> j
noremap  k
noremap  l
map  :q
map  :w
map  :bp :bd # 
map ,c :!ctags -R include src
map ,t :set tags+=~/Development/Tags/
map Q gq
map \mbt <Plug>TMiniBufExplorer
map \mbu <Plug>UMiniBufExplorer
map \mbc <Plug>CMiniBufExplorer
map \mbe <Plug>MiniBufExplorer
nmap <silent> \ucs :call C_RemoveGuiMenus()
nmap <silent> \lcs :call C_CreateGuiMenus()
nmap gx <Plug>NetrwBrowseX
nnoremap <silent> <Plug>NetrwBrowseX :call netrw#NetrwBrowseX(expand("<cWORD>"),0)
map <F11> :NERDTree :vertical resize 23
map <F10> :make -j3 CXX="ccache g++" CONFIG=Debug
map <F2> :bnext
map <F1> :bprevious
map <C-CR> :!urxvt &
imap 	 
inoremap  u
imap  =CtrlXPP()
let &cpo=s:cpo_save
unlet s:cpo_save
set background=dark
set backspace=indent,eol,start
set noequalalways
set expandtab
set fileencodings=ucs-bom,utf-8,default,latin1
set guifont=Monospace\ 9
set guioptions=aegimrLt
set helplang=en
set history=50
set hlsearch
set ignorecase
set incsearch
set mouse=a
set ruler
set shiftwidth=4
set showcmd
set tags=./tags,./TAGS,tags,TAGS,~/Development/Tags/ogre,~/Development/Tags/bullet,~/Development/Tags/boostpython,~/Development/Tags/btogre,~/Development/Tags/mygui,~/Development/Tags/ngf,~/Development/Tags/ois
set termencoding=utf-8
set window=65
" vim: set ft=vim :
