" Vim-plug Setup
" 
call plug#begin('~/.vim/plugged')
Plug 'tpope/vim-sensible'
Plug 'junegunn/seoul256.vim'
Plug 'scrooloose/nerdtree'
Plug 'kien/ctrlp.vim'
call plug#end()

" Key Bindings
"
noremap <C-C> "+y
map     <C-V> "+gP
cmap    <C-V> <C-R>+

" Misc Settings
"
set autoindent
set whichwrap+=<,>,[,]
colo seoul256
autocmd vimenter * NERDTree
let NERDTreeShowBookmarks=1
set list
set listchars=tab:>-
let g:ctrlp_show_hidden=1
filetype plugin indent on
set tabstop=4
set shiftwidth=4
set expandtab

" Disabled Misc Settings
" autocmd vimenter * NERDTree
