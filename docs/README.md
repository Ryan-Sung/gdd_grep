# gdd_grep
Was a training for regex, but it's cool and interesting to develops to a grep implementation

Environments:
1. 在 '\~/.zshrc' 加入 'export PATH=\~/bin:$PATH' (依 shell 種類修改 .bashrc 等)
2. 將程式執行檔(exe, Mach-O)放入 '~/bin'
2. 記得重啟視窗或 source

details:
1. 目前先把 {n, m} 做成 n 個 '+' 加上 (m-n) 個 '?'，未來視情況引入高效率工具如 counter-based nfa