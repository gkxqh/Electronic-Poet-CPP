// 页面加载完成后执行
window.addEventListener('DOMContentLoaded', () => {
    // 获取DOM元素
    const generateBtn = document.getElementById('generate-btn');
    const linesInput = document.getElementById('lines');
    const poemContent = document.getElementById('poem-content');
    const saveGroup = document.getElementById('save-group');
    const titleInput = document.getElementById('title');
    const saveBtn = document.getElementById('save-btn');
    const poemList = document.getElementById('poem-list');
    const generateNavBtn = document.getElementById('generate-nav-btn');
    const historyNavBtn = document.getElementById('history-nav-btn');
    const themeToggle = document.getElementById('theme-toggle');
    const themeIcon = themeToggle.querySelector('i');
    const importBtn = document.getElementById('import-btn');
    const importFile = document.getElementById('import-file');
    
    // 初始化：加载历史诗歌列表
    loadPoems();
    
    // 初始化：设置主题
    initTheme();
    
    // 初始化：添加鼠标跟随效果
    initMouseFollow();
    
    // 生成诗歌按钮点击事件
    generateBtn.addEventListener('click', () => {
        const lines = linesInput.value;
        generatePoetry(lines);
    });
    
    // 保存诗歌按钮点击事件
    saveBtn.addEventListener('click', () => {
        const title = titleInput.value.trim();
        if (title) {
            savePoetry(title);
        } else {
            alert('请输入诗歌标题！');
        }
    });
    
    // 菜单栏生成诗歌按钮点击事件
    generateNavBtn.addEventListener('click', () => {
        document.querySelector('.generate-section').scrollIntoView({ behavior: 'smooth' });
    });
    
    // 菜单栏历史诗歌按钮点击事件
    historyNavBtn.addEventListener('click', () => {
        document.querySelector('.history-section').scrollIntoView({ behavior: 'smooth' });
    });
    
    // 主题切换按钮点击事件
    themeToggle.addEventListener('click', () => {
        toggleTheme();
    });
    
    // 导入按钮点击事件
    importBtn.addEventListener('click', () => {
        importFile.click();
    });
    
    // 文件选择事件
    importFile.addEventListener('change', (e) => {
        handleFileImport(e.target.files[0]);
    });
    
    // 生成诗歌函数
    async function generatePoetry(lines) {
        try {
            // 显示加载状态
            poemContent.innerHTML = '<p style="color: var(--primary-color); font-weight: 600;">电子诗人正在创作中...</p>';
            
            // 发送请求到后端
            const response = await fetch('/generate', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/x-www-form-urlencoded',
                },
                body: new URLSearchParams({ lines })
            });
            
            // 解析响应
            const data = await response.json();
            
            if (data.status === 'success') {
                // 显示生成的诗歌
                displayPoem(data.data);
                // 显示保存按钮
                saveGroup.style.display = 'flex';
            } else {
                poemContent.innerHTML = `<p style="color: #ef4444; font-weight: 600;">生成失败：${data.message}</p>`;
                saveGroup.style.display = 'none';
            }
        } catch (error) {
            console.error('生成诗歌出错：', error);
            poemContent.innerHTML = '<p style="color: #ef4444; font-weight: 600;">生成失败，请检查网络连接！</p>';
            saveGroup.style.display = 'none';
        }
    }
    
    // 显示诗歌函数
    function displayPoem(poem) {
        poemContent.innerHTML = '';
        poem.forEach(line => {
            const p = document.createElement('p');
            p.textContent = line;
            poemContent.appendChild(p);
        });
    }
    
    // 保存诗歌函数
    async function savePoetry(title) {
        try {
            // 发送请求到后端
            const response = await fetch('/save', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/x-www-form-urlencoded',
                },
                body: new URLSearchParams({ title })
            });
            
            // 解析响应
            const data = await response.json();
            
            if (data.status === 'success') {
                alert('诗歌保存成功！');
                // 清空标题输入框
                titleInput.value = '';
                // 隐藏保存按钮
                saveGroup.style.display = 'none';
                // 重新加载诗歌列表
                loadPoems();
            } else {
                alert(`保存失败：${data.message}`);
            }
        } catch (error) {
            console.error('保存诗歌出错：', error);
            alert('保存失败，请检查网络连接！');
        }
    }
    
    // 删除诗歌函数
    async function deletePoetry(title) {
        if (confirm(`确定要删除诗歌《${title}》吗？`)) {
            try {
                const response = await fetch(`/poem/${encodeURIComponent(title)}`, {
                    method: 'DELETE'
                });
                
                const data = await response.json();
                if (data.status === 'success') {
                    alert('诗歌删除成功！');
                    loadPoems();
                } else {
                    alert(`删除失败：${data.message}`);
                }
            } catch (error) {
                console.error('删除诗歌出错：', error);
                alert('删除失败，请检查网络连接！');
            }
        }
    }
    
    // 导入诗歌函数
    function handleFileImport(file) {
        if (!file) return;
        
        if (file.type !== 'text/plain') {
            alert('请选择文本文件(.txt)！');
            return;
        }
        
        const reader = new FileReader();
        reader.onload = async (e) => {
            const content = e.target.result;
            const lines = content.split('\n').filter(line => line.trim() !== '');
            
            if (lines.length === 0) {
                alert('文件内容为空！');
                return;
            }
            
            // 让用户输入诗歌标题
            const title = prompt('请输入诗歌标题：', file.name.replace('.txt', ''));
            if (!title) {
                return; // 用户取消输入
            }
            
            if (title.trim() === '') {
                alert('标题不能为空！');
                return;
            }
            
            try {
                // 使用新的导入API端点
                const response = await fetch('/import', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/x-www-form-urlencoded',
                    },
                    body: new URLSearchParams({ 
                        title: title, 
                        content: content 
                    })
                });
                
                const data = await response.json();
                if (data.status === 'success') {
                    alert('诗歌导入成功！');
                    loadPoems();
                } else {
                    alert(`导入失败：${data.message}`);
                }
            } catch (error) {
                console.error('导入诗歌出错：', error);
                alert('导入失败，请检查网络连接！');
            }
        };
        
        reader.readAsText(file);
        importFile.value = ''; // 重置文件输入
    }
    
    // 加载诗歌列表函数
    async function loadPoems() {
        try {
            // 发送请求到后端
            const response = await fetch('/poems', {
                method: 'GET'
            });
            
            // 解析响应
            const data = await response.json();
            
            if (data.status === 'success') {
                displayPoemList(data.data);
            } else {
                console.error('加载诗歌列表失败：', data.message);
            }
        } catch (error) {
            console.error('加载诗歌列表出错：', error);
        }
    }
    
    // 显示诗歌列表函数
    function displayPoemList(poems) {
        poemList.innerHTML = '';
        
        if (poems.length === 0) {
            poemList.innerHTML = '<p style="color: var(--accent-color); text-align: center; width: 100%;">暂无保存的诗歌</p>';
            return;
        }
        
        poems.forEach(title => {
            // 创建诗歌项元素
            const poemItem = document.createElement('div');
            poemItem.className = 'poem-item';
            
            // 获取诗歌预览内容
            getPoemPreview(title).then(preview => {
                poemItem.innerHTML = `
                    <h3>
                        ${title}
                        <button class="delete-btn" data-title="${title}">
                            <i class="fas fa-trash"></i>
                        </button>
                    </h3>
                    <div class="poem-preview">${preview}</div>
                `;
                
                // 添加点击事件：显示完整诗歌内容
                poemItem.addEventListener('click', (e) => {
                    if (!e.target.closest('.delete-btn')) {
                        showPoemContent(title);
                    }
                });
                
                // 添加删除按钮点击事件
                const deleteBtn = poemItem.querySelector('.delete-btn');
                deleteBtn.addEventListener('click', (e) => {
                    e.stopPropagation();
                    deletePoetry(title);
                });
                
                // 添加到诗歌列表
                poemList.appendChild(poemItem);
            });
        });
    }
    
    // 获取诗歌预览内容函数
    async function getPoemPreview(title) {
        try {
            const response = await fetch(`/poem/${encodeURIComponent(title)}`, {
                method: 'GET'
            });
            
            const data = await response.json();
            
            if (data.status === 'success' && data.data.length > 0) {
                // 返回前两行作为预览
                return data.data.slice(0, 2).join('\n');
            } else {
                return '暂无内容';
            }
        } catch (error) {
            console.error('获取诗歌预览出错：', error);
            return '加载失败';
        }
    }
    
    // 显示完整诗歌内容函数
    async function showPoemContent(title) {
        try {
            const response = await fetch(`/poem/${encodeURIComponent(title)}`, {
                method: 'GET'
            });
            
            const data = await response.json();
            
            if (data.status === 'success') {
                // 显示诗歌内容
                displayPoem(data.data);
                // 滚动到诗歌展示区域
                document.getElementById('poem-section').scrollIntoView({ behavior: 'smooth' });
            } else {
                alert(`获取诗歌内容失败：${data.message}`);
            }
        } catch (error) {
            console.error('获取诗歌内容出错：', error);
            alert('获取诗歌内容失败，请检查网络连接！');
        }
    }
    
    // 初始化主题
    function initTheme() {
        const savedTheme = localStorage.getItem('theme') || 'system';
        applyTheme(savedTheme);
    }
    
    // 应用主题
    function applyTheme(theme) {
        const root = document.documentElement;
        
        if (theme === 'system') {
            const systemPrefersDark = window.matchMedia('(prefers-color-scheme: dark)').matches;
            root.setAttribute('data-theme', systemPrefersDark ? 'dark' : 'light');
            themeIcon.className = systemPrefersDark ? 'fas fa-sun' : 'fas fa-moon';
        } else {
            root.setAttribute('data-theme', theme);
            themeIcon.className = theme === 'dark' ? 'fas fa-sun' : 'fas fa-moon';
        }
        
        localStorage.setItem('theme', theme);
    }
    
    // 切换主题
    function toggleTheme() {
        const currentTheme = localStorage.getItem('theme') || 'system';
        let newTheme;
        
        switch (currentTheme) {
            case 'light':
                newTheme = 'dark';
                break;
            case 'dark':
                newTheme = 'light';
                break;
            case 'system':
                newTheme = 'dark';
                break;
            default:
                newTheme = 'light';
        }
        
        applyTheme(newTheme);
    }
    
    // 初始化鼠标跟随效果
    function initMouseFollow() {
        document.addEventListener('mousemove', (e) => {
            const x = e.clientX / window.innerWidth * 100;
            const y = e.clientY / window.innerHeight * 100;
            
            document.body.style.setProperty('--mouse-x', `${x}%`);
            document.body.style.setProperty('--mouse-y', `${y}%`);
        });
    }
    
    // 键盘事件：回车键生成诗歌
    linesInput.addEventListener('keypress', (e) => {
        if (e.key === 'Enter') {
            generateBtn.click();
        }
    });
    
    // 键盘事件：回车键保存诗歌
    titleInput.addEventListener('keypress', (e) => {
        if (e.key === 'Enter') {
            saveBtn.click();
        }
    });
    
    // 监听系统主题变化
    window.matchMedia('(prefers-color-scheme: dark)').addEventListener('change', () => {
        const savedTheme = localStorage.getItem('theme');
        if (savedTheme === 'system') {
            initTheme();
        }
    });
});