(() => {

/* ══════════════════════════════════════
   ASCII CANVAS
══════════════════════════════════════ */
const canvas = document.getElementById('ascii-canvas');
if (canvas) {
    const ctx = canvas.getContext('2d');
    const CHARS_RICH  = '░▒▓│┤╡╢╖╕╣║╗╝╜╛┐└┴┬├─┼╞╟╚╔╩╦╠═╬╧╨╤╥╙╘╒╓╫╪┘┌█▄▌▐▀αßΓπΣσµτΦΘΩδ∞φε∩≡±≥≤⌠⌡÷≈°∙·√ⁿ²';
    const CHARS_PUNCT = '.,\';:!?/\\|[]{}()<>@#$%^&*-_=+~`"';
    const ALL_CHARS   = CHARS_RICH + CHARS_PUNCT;
    const MAX_COLS = 160, MAX_ROWS = 90;
    let W, H, cols, rows, grid, ages, effectiveFW, effectiveFH;
    let mouseX = -999, mouseY = -999, hasMoused = false, t = 0;

    const randomChar = () => ALL_CHARS[Math.floor(Math.random() * ALL_CHARS.length)];

    function resize() {
        W = window.innerWidth; H = window.innerHeight;
        canvas.width = W; canvas.height = H;
        cols = Math.min(Math.ceil(W / 7), MAX_COLS);
        rows = Math.min(Math.ceil(H / 13), MAX_ROWS);
        effectiveFW = W / cols; effectiveFH = H / rows;
        grid = []; ages = [];
        for (let r = 0; r < rows; r++) {
            grid[r] = []; ages[r] = [];
            for (let c = 0; c < cols; c++) { grid[r][c] = randomChar(); ages[r][c] = Math.random() * 60; }
        }
    }

    window.addEventListener('mousemove', e => { mouseX = e.clientX; mouseY = e.clientY; hasMoused = true; });
    window.addEventListener('mouseleave', () => { hasMoused = false; });
    window.addEventListener('touchmove', e => { mouseX = e.touches[0].clientX; mouseY = e.touches[0].clientY; hasMoused = true; }, { passive: true });
    window.addEventListener('touchend', () => { hasMoused = false; });

    function noise(x, y, t) {
        return (
            Math.sin(x * 0.13 + t * 0.7) * Math.cos(y * 0.11 - t * 0.5) +
            Math.sin((x + y) * 0.08 + t * 0.3) +
            Math.sin(x * 0.05 - t * 0.2) * Math.cos(y * 0.17 + t * 0.4)
        );
    }

    function draw() {
        t += 0.016;
        ctx.clearRect(0, 0, W, H);
        const fontSize = Math.max(7, Math.floor(effectiveFH * 0.82));
        ctx.font = `${fontSize}px 'IBM Plex Mono','Courier New',monospace`;
        ctx.textBaseline = 'top';
        const mc = mouseX / effectiveFW, mr = mouseY / effectiveFH;
        for (let r = 0; r < rows; r++) {
            for (let c = 0; c < cols; c++) {
                const n = noise(c, r, t), intensity = (n + 2) / 4;
                const distCenter = Math.hypot(c - cols / 2, r - rows / 2);
                const centerPull = Math.max(0, 1 - distCenter / (Math.min(cols, rows) * 0.42));
                let mousePull = hasMoused ? Math.max(0, 1 - Math.hypot(c - mc, r - mr) / 10) : 0;
                ages[r][c] += 0.35 + n * 0.25 + mousePull * 1.8 + centerPull * 0.4;
                if (ages[r][c] > 18 + Math.sin(c * 0.2 + r * 0.15 + t) * 8) { ages[r][c] = 0; grid[r][c] = randomChar(); }
                const alpha = Math.max(0.025, Math.min(0.55, intensity * 0.27 + mousePull * 0.12 + centerPull * 0.20));
                if (intensity > 0.80 || mousePull > 0.65) {
                    ctx.fillStyle = `rgba(255,252,240,${Math.min(alpha * 1.8, 0.85).toFixed(3)})`;
                } else if (intensity > 0.62) {
                    ctx.fillStyle = `rgba(230,235,255,${(alpha * 1.25).toFixed(3)})`;
                } else {
                    const wR = Math.floor(140 + Math.sin(c * 0.18 + t * 1.1) * 40);
                    const wG = Math.floor(175 + Math.sin(r * 0.14 - t * 0.9) * 30);
                    const wB = Math.floor(195 + Math.sin(c * 0.09 + r * 0.12 + t) * 25);
                    ctx.fillStyle = `rgba(${wR},${wG},${wB},${alpha.toFixed(3)})`;
                }
                ctx.fillText(grid[r][c], c * effectiveFW, r * effectiveFH);
            }
        }
        requestAnimationFrame(draw);
    }
    resize();
    window.addEventListener('resize', resize);
    requestAnimationFrame(draw);
}

/* ══════════════════════════════════════
   TYPEWRITER — Hero (index.html)
   FIX: cursor disparaît un character avant la fin
══════════════════════════════════════ */
const elTerminal = document.getElementById('title-terminal');
const elRenderer = document.getElementById('title-renderer');
const elCursor   = document.getElementById('title-cursor');

if (elTerminal && elRenderer) {
    const fullText = 'Terminal-Renderer', splitAt = 'Terminal-'.length;
    let idx = 0;
    function typeNext() {
        if (idx <= splitAt) elTerminal.textContent = fullText.slice(0, idx);
        else { elTerminal.textContent = fullText.slice(0, splitAt); elRenderer.textContent = fullText.slice(splitAt, idx); }
        // Cacher le curseur un character avant la fin
        if (idx === fullText.length - 1 && elCursor) elCursor.style.display = 'none';
        idx++;
        if (idx <= fullText.length) setTimeout(typeNext, 80);
        else setTimeout(() => { elRenderer.classList.add('shimmer-text'); }, 400);
    }
    setTimeout(typeNext, 900);
}

/* ══════════════════════════════════════
   3D TILT — Module cards + Render system cards
══════════════════════════════════════ */
const MAX_TILT = 14, PERSP = 600;

function applyTilt(selector) {
    document.querySelectorAll(selector).forEach(card => {
        card.addEventListener('mousemove', e => {
            const rect = card.getBoundingClientRect();
            const dx = (e.clientX - rect.left - rect.width / 2) / (rect.width / 2);
            const dy = (e.clientY - rect.top - rect.height / 2) / (rect.height / 2);
            card.style.transform = `perspective(${PERSP}px) rotateX(${-dy * MAX_TILT}deg) rotateY(${dx * MAX_TILT}deg) translateZ(10px)`;
            card.style.transition = 'transform 0.08s ease, border-color 0.3s, box-shadow 0.3s';
            card.style.setProperty('--mx', ((e.clientX - rect.left) / rect.width * 100).toFixed(1) + '%');
            card.style.setProperty('--my', ((e.clientY - rect.top) / rect.height * 100).toFixed(1) + '%');
        });
        card.addEventListener('mouseleave', () => {
            card.style.transform = `perspective(${PERSP}px) rotateX(0deg) rotateY(0deg) translateZ(0px)`;
            card.style.transition = 'transform 0.5s ease, border-color 0.3s, box-shadow 0.3s';
        });
    });
}

applyTilt('.module-item');
applyTilt('.render-system');

/* ══════════════════════════════════════
   i18n — TRANSLATIONS
   (FR = texte natif déjà présent dans le HTML, pas besoin de l'objet T)
══════════════════════════════════════ */
const T = {
    en: {
        'nav.home': 'Home', 'nav.modules': 'Modules', 'nav.widg': '.widg Language',
        'nav.engine': 'Engine', 'nav.roadmap': 'Roadmap',
        'nav.doc': 'Outil WIDG', 'nav.about': 'About',
        'hero.eyebrow': 'Open-source terminal rendering engine',
        'hero.sub': 'A modular terminal rendering & UI engine — dynamic widgets, smooth animations, .widg language',
        'hero.btn.explore': 'Explore →', 'hero.btn.github': 'GitHub ↗',
        'hero.pill.dev': '🟢 In Development',
        's01.num': '01 / Overview', 's01.title': 'The terminal as a<br>graphics canvas',
        's01.stat.modules': 'Modules', 's01.stat.render': 'Render systems',
        's01.stat.version': 'Version', 's01.stat.iter': 'Iterations',
        's01.p1': 'Terminal-Renderer is a C++ library providing a complete rendering and interaction infrastructure in the terminal — dynamic text widgets, smooth animations, a proprietary XML language, with no external graphics dependencies.',
        's01.p2': 'All objects follow a polymorphic hierarchy rooted at <code>trObject</code>, with custom weak pointers for precise memory management without external dependencies.',
        's01.p3': 'Performance is a constant goal: run the engine even on a toaster. Every optimization counts.',
        's01.quote': '"When the terminal becomes your canvas, every character counts."',
        's01.f1.title': 'Multi-layer Rendering', 's01.f1.desc': 'Partial refresh of modified areas — no unnecessary full redraws. Pipeline optimized for limited hardware.',
        's01.f2.title': 'Delta Time', 's01.f2.desc': 'Animations and transitions synchronized independently of framerate. Smooth movement and dynamic highlight effects.',
        's01.f3.title': '.widg Language', 's01.f3.desc': 'Proprietary XML format to describe, load and live-preview widgets without recompilation.',
        's01.f4.title': 'Python Precompiler', 's01.f4.desc': 'Automatically detects classes inheriting from <code>trActor</code> and inserts <code>REGISTER_TYPE</code> macros without manual intervention.',
        's02.num': '02 / Architecture', 's02.title': 'Modular<br>Architecture',
        'mod.core.desc': 'Internal structures, core types — <code>trPair</code>, <code>trMulti</code>, <code>trMap</code>…',
        'mod.engine.desc': 'Engine core, main logic, inter-module communication',
        'mod.render.desc': 'Render pipeline, positioning, multi-layer compositing',
        'mod.ui.desc': 'Widget hierarchy system, focus management, hide/show',
        'mod.comp.desc': 'Base components — buttons, text, containers, progress bars',
        'mod.input.desc': 'Keyboard input management, multithread buffer, non-blocking mode',
        'mod.world.desc': 'Global context, scene management and environment logic',
        'mod.load.desc': 'Parsing of .widg files (proprietary XML), live preview',
        'mod.tool.desc': 'Math toolkit, algorithmic tools and utilities',
        'mod.audio.desc': 'Audio management and notifications in the terminal',
        'mod.print.desc': 'Text printing and full ANSI color management',
        'status.stable': '🟢 Stable', 'status.func': '🟢 Functional',
        'status.wip': '🟡 In Progress', 'status.todo': '🟡 To Develop', 'status.unstable': '🔴 Unstable',
        's03.num': '03 / .widg Language', 's03.title': 'Proprietary<br>XML Format',
        's03.p1': 'Each widget can be described via a <code>.widg</code> file using custom XML with advanced tags. These tags define the properties, position, content and colors of a widget.',
        's03.p2': 'A key advantage of the format: live editing. Press <code>F5/F6/F8/F9/F10</code> in the preview to see XML changes immediately — no recompilation.',
        's03.quote': 'Live editing · Animations · Escape sequences · Per-interval ANSI colors',
        's03.f1.title': 'Dynamic Loading', 's03.f1.desc': 'Open and visualize any .widg file standalone with all animations and colors, in complete isolation.',
        's03.f2.title': 'Real-time Debugging', 's03.f2.desc': 'Modify XML and see changes instantly — ideal for complex animated widgets.',
        's03.f3.title': 'Frame-by-frame Animation', 's03.f3.desc': 'Each <code>trText</code> widget supports animation containers with timestamped frames and automatic transitions.',
        's03.cta.badge': '🛠 Interactive Tool', 's03.cta.title': 'Preview and create your .widg files directly in the browser',
        's03.cta.desc': 'Import .widg files, live editor, XML generator — no installation required.',
        's03.cta.btn': 'Open the Tool →',
        's03.th.elem': 'XML Element', 's03.th.attr': 'Attribute(s)', 's03.th.type': 'C++ Type', 's03.th.desc': 'Description',
        's03.note': '⚠️ The table is not complete — the .widg format is still in active development. Contact the author for more information.',
        's04.num': '04 / Special Characters', 's04.title': 'Sequence<br>Handling',
        's04.p1': 'The engine natively handles a set of escape sequences and special characters. Each can have unpredictable visual effects depending on the target terminal.',
        's04.p2': 'Handling these cases is especially critical in <code>RENDER_SYSTEM</code> mode where all rendering passes through a complete buffer before display.',
        's04.quote': 'Windows / Linux compatibility tested — OS-dependent behaviors identified and handled.',
        'seq.n.desc': 'Line break — can break formatting if uncontrolled',
        'seq.r.desc': 'Carriage return — may overwrite an existing line depending on OS',
        'seq.t.desc': 'Horizontal tab — variable spacing depending on terminal',
        'seq.b.desc': 'Backspace — unpredictable visual effect, monitor carefully',
        'seq.v.desc': 'Vertical tab — may misalign rendering',
        'seq.f.desc': 'Form feed — rarely used but handled',
        'seq.0.desc': 'Null character — watch out in buffers',
        'seq.bs.desc': 'Backslash — must be properly escaped',
        's05.num': '05 / Render Engine', 's05.title': 'Three Render<br>Systems',
        'render.direct': 'Direct System', 'render.direct.tag': 'Slow',
        'render.direct.desc': 'Each character written directly to the terminal. Simple but very slow — causes lag and visual bugs at scale.',
        'render.buffer': 'Buffer System', 'render.buffer.tag': 'Medium',
        'render.buffer.desc': 'Everything written to an <code>ostringstream</code> buffer before display. Smooth for few actions, unstable with many simultaneous changes.',
        'render.system': 'Render System', 'render.system.tag': 'Optimal',
        'render.system.desc': 'Complete buffer without touching the terminal directly. Most performant — full position management, layering and pre-display modifications.',
        's05.p1': 'The engine supports a full ANSI color system — foreground, background, highlights, dynamic transitions — across all widgets.',
        's05.p2': 'A Python precompiler automatically scans <code>.h/.cpp</code> files to detect classes inheriting from <code>trActor</code> and inserts <code>REGISTER_TYPE</code> macros.',
        's05.p3': 'The widget collision system allows retrieving intersections and defining desired reactions — overlap, bounce, priority masking.',
        's05.quote': 'Live widget editing via F5 / F6 / F8 / F9 / F10 — no recompilation.',
        's06.num': '06 / Roadmap', 's06.title': 'Next<br>Steps',
        'road.f1.title': 'Mouse System', 'road.f1.desc': 'Position detection, clicks and hover events directly in the terminal.',
        'road.f2.title': 'Audio API', 'road.f2.desc': 'Sound effects and audio notifications integrated into the widget engine via AudioModule.',
        'road.f3.title': 'Example Projects', 'road.f3.desc': 'Fractal simulation, T-Rex game, complete mini UI with interactive widgets.',
        'road.f4.title': 'Template Repo', 'road.f4.desc': 'Ready-to-use repository to start your own Terminal-Renderer project in minutes.',
        'road.f5.title': 'Full Documentation', 'road.f5.desc': 'Internal Doxygen + detailed Markdown for each module and .widg tag.',
        'road.f6.title': 'Memory Profiling', 'road.f6.desc': 'Built-in memory and CPU profiling to diagnose hotspots in production.',
        's06.p1': 'The long-term vision is to become a complete open-source console rendering engine, modular and extensible — inspired by Unreal Engine architectures but designed for a purely text environment.',
        's06.p2': 'The performance goal remains constant: run the engine even on a toaster. Every optimization counts, every allocation is measured.',
        's06.p3': 'The <code>ContentReorganisation()</code> module is being optimized to reduce unnecessary recalculations during partial buffer updates.',
        'bottom.doc': 'Outil WIDG', 'bottom.about': 'About me',
        'doc.hero.eyebrow': 'Tools & Reference',
        'doc.hero.sub': 'Import, visualize, edit and create .widg files directly in the browser',
        'doc.s01.num': '01 / Preview', 'doc.s01.title': 'Import &amp;<br>Edit a .widg',
        'doc.s02.num': '02 / Creator', 'doc.s02.title': 'Create a .widg<br>from scratch',
        'about.eyebrow': 'About me', 'about.sub': 'C++ Developer · Creator of Terminal-Renderer',
        'about.bio1': 'Passionate about low-level systems, terminal rendering, and software architecture. Terminal-Renderer was born from the belief that the terminal can be far more than a simple text tool.',
        'about.bio2': 'The entire engine infrastructure is designed with no external graphics dependencies — polymorphism, custom memory management, optimized render pipeline.',
        'about.bio3': 'Written in C++20/C++23, encoded in UTF-8, targeting compatibility with Windows & Linux.',
        'about.quote': '"Work with constraints, not despite them."',
        'about.card.tech': 'Tech Stack', 'about.card.project': 'The Project',
        'about.card.timeline': 'Timeline',
        'about.github': 'GitHub — RydeProgramation ↗', 'about.contact': 'Contact',
        'about.scroll': 'Scroll',
    },
    ar: {
        'nav.home': 'الرئيسية', 'nav.modules': 'الوحدات', 'nav.widg': 'لغة .widg',
        'nav.engine': 'المحرك', 'nav.roadmap': 'خارطة الطريق',
        'nav.doc': 'التوثيق', 'nav.about': 'عني',
        'hero.eyebrow': 'محرك عرض طرفي مفتوح المصدر',
        'hero.sub': 'محرك معياري لعرض وتفاعل الطرفية — واجهات ديناميكية، رسوم متحركة سلسة، لغة .widg',
        'hero.btn.explore': 'استكشف ←', 'hero.btn.github': 'GitHub ↗',
        'hero.pill.dev': '🟢 قيد التطوير',
        's01.num': '٠١ / نظرة عامة', 's01.title': 'الطرفية كلوحة<br>رسومية',
        's01.stat.modules': 'وحدات', 's01.stat.render': 'أنظمة عرض',
        's01.stat.version': 'الإصدار', 's01.stat.iter': 'تكرارات',
        's01.p1': 'Terminal-Renderer هي مكتبة C++ تهدف إلى توفير بنية تحتية متكاملة للعرض والتفاعل في الطرفية — واجهات نصية ديناميكية، رسوم متحركة سلسة، لغة XML مخصصة، دون أي تبعيات رسومية خارجية.',
        's01.p2': 'جميع الكائنات مرتبة وفق تسلسل بولي مورفي ينطلق من <code>trObject</code>، مع مؤشرات ضعيفة مخصصة لإدارة دقيقة للذاكرة.',
        's01.p3': 'هدف الأداء ثابت: تشغيل المحرك حتى على أضعف الأجهزة. كل تحسين يُحسب.',
        's01.quote': '"حين تصبح الطرفية لوحتك، كل حرف له قيمة."',
        's01.f1.title': 'عرض متعدد الطبقات', 's01.f1.desc': 'تحديث جزئي للمناطق المعدّلة فقط — دون إعادة رسم كاملة غير ضرورية.',
        's01.f2.title': 'دلتا الوقت', 's01.f2.desc': 'رسوم متحركة وانتقالات متزامنة بغض النظر عن معدل الإطارات.',
        's01.f3.title': 'لغة .widg', 's01.f3.desc': 'صيغة XML مخصصة لوصف وتحميل ومعاينة الواجهات مباشرة دون إعادة تصريف.',
        's01.f4.title': 'مُعالِج Python المسبق', 's01.f4.desc': 'يكتشف تلقائيًا الفئات الوارثة من <code>trActor</code> ويُدرج ماكرو <code>REGISTER_TYPE</code>.',
        's02.num': '٠٢ / البنية', 's02.title': 'بنية<br>معيارية',
        'mod.core.desc': 'الهياكل الداخلية والأنواع الأساسية — <code>trPair</code>، <code>trMulti</code>، <code>trMap</code>…',
        'mod.engine.desc': 'نواة المحرك، المنطق الرئيسي، التواصل بين الوحدات',
        'mod.render.desc': 'خط أنابيب العرض، التموضع، الطبقات المتعددة',
        'mod.ui.desc': 'نظام تسلسل الواجهات، إدارة التركيز، الإخفاء/الإظهار',
        'mod.comp.desc': 'المكونات الأساسية — أزرار، نصوص، حاويات، أشرطة تقدم',
        'mod.input.desc': 'إدارة إدخالات لوحة المفاتيح، مخزن مؤقت متعدد الخيوط',
        'mod.world.desc': 'السياق العام وإدارة المشهد والمنطق البيئي',
        'mod.load.desc': 'تحليل ملفات .widg (XML مخصص)، معاينة مباشرة',
        'mod.tool.desc': 'أدوات رياضية وخوارزميات ومساعدات متنوعة',
        'mod.audio.desc': 'إدارة الصوت والإشعارات الصوتية في الطرفية',
        'mod.print.desc': 'طباعة النص وإدارة ألوان ANSI الكاملة',
        'status.stable': '🟢 مستقر', 'status.func': '🟢 يعمل',
        'status.wip': '🟡 قيد التطوير', 'status.todo': '🟡 للتطوير', 'status.unstable': '🔴 غير مستقر',
        's03.num': '٠٣ / لغة .widg', 's03.title': 'صيغة XML<br>مخصصة',
        's03.p1': 'يمكن وصف كل واجهة عبر ملف <code>.widg</code> باستخدام XML مخصص بوسوم متقدمة. هذه الوسوم تحدد الخصائص والموضع والمحتوى وألوان الواجهة.',
        's03.p2': 'ميزة رئيسية: التعديل المباشر. بالضغط على <code>F5/F6/F8/F9/F10</code> تظهر التغييرات فورًا دون إعادة تصريف.',
        's03.quote': 'تحرير مباشر · رسوم متحركة · تسلسلات الهروب · ألوان ANSI بالفاصل',
        's03.f1.title': 'تحميل ديناميكي', 's03.f1.desc': 'فتح ومعاينة أي ملف .widg مع رسوماته وألوانه بالكامل.',
        's03.f2.title': 'تصحيح فوري', 's03.f2.desc': 'تعديل XML ورؤية التغييرات لحظيًا — مثالي للواجهات المتحركة المعقدة.',
        's03.f3.title': 'رسوم إطار بإطار', 's03.f3.desc': 'كل واجهة <code>trText</code> تدعم حاويات رسوم متحركة بإطارات موقوتة.',
        's03.cta.badge': '🛠 أداة تفاعلية', 's03.cta.title': 'عاين وأنشئ ملفات .widg مباشرة في المتصفح',
        's03.cta.desc': 'استيراد ملفات .widg، محرر مباشر، مولّد XML — بدون تثبيت.',
        's03.cta.btn': 'فتح الأداة ←',
        's03.th.elem': 'عنصر XML', 's03.th.attr': 'السمة/السمات', 's03.th.type': 'نوع C++', 's03.th.desc': 'الوصف',
        's03.note': '⚠️ الجدول غير مكتمل — صيغة .widg لا تزال قيد التطوير النشط.',
        's04.num': '٠٤ / أحرف خاصة', 's04.title': 'التعامل مع<br>التسلسلات',
        's04.p1': 'يتعامل المحرك بشكل أصلي مع مجموعة من تسلسلات الهروب والأحرف الخاصة. كل منها قد يكون له تأثيرات بصرية غير متوقعة حسب الطرفية المستهدفة.',
        's04.p2': 'التعامل مع هذه الحالات أمر بالغ الأهمية في وضع <code>RENDER_SYSTEM</code> حيث يمر كل العرض عبر مخزن مؤقت كامل قبل العرض.',
        's04.quote': 'تم اختبار التوافق مع Windows / Linux — تم تحديد السلوكيات المعتمدة على نظام التشغيل ومعالجتها.',
        'seq.n.desc': 'سطر جديد — قد يكسر التنسيق', 'seq.r.desc': 'عودة العجلة — قد يطغى على سطر موجود',
        'seq.t.desc': 'مسافة أفقية — تباين المسافات', 'seq.b.desc': 'مسافة خلفية — تأثير بصري غير متوقع',
        'seq.v.desc': 'مسافة رأسية — قد تُخلّ بالمحاذاة', 'seq.f.desc': 'تغذية النماذج — نادر لكنه مُعالَج',
        'seq.0.desc': 'الحرف الخالي — يستدعي المراقبة', 'seq.bs.desc': 'الشرطة المائلة العكسية — يجب تهريبها',
        's05.num': '٠٥ / محرك العرض', 's05.title': 'ثلاثة أنظمة<br>للعرض',
        'render.direct': 'النظام المباشر', 'render.direct.tag': 'بطيء',
        'render.direct.desc': 'كل حرف يُكتب مباشرة على الطرفية. بسيط لكن بطيء جدًا.',
        'render.buffer': 'نظام المخزن المؤقت', 'render.buffer.tag': 'متوسط',
        'render.buffer.desc': 'يُكتب كل شيء في مخزن <code>ostringstream</code> قبل العرض.',
        'render.system': 'نظام العرض', 'render.system.tag': 'مثالي',
        'render.system.desc': 'مخزن مؤقت كامل دون لمس الطرفية مباشرة. الأكثر أداءً.',
        's05.p1': 'يدعم المحرك نظام ألوان ANSI كاملًا — foreground وbackground وإضاءة وانتقالات ديناميكية.',
        's05.p2': 'يفحص مُعالِج Python المسبق تلقائيًا ملفات <code>.h/.cpp</code> للكشف عن الفئات الوارثة من <code>trActor</code>.',
        's05.p3': 'نظام التصادم بين الواجهات يتيح استرداد التقاطعات وتحديد ردود الفعل المطلوبة.',
        's05.quote': 'تحرير الواجهات مباشرة عبر F5 / F6 / F8 / F9 / F10.',
        's06.num': '٠٦ / خارطة الطريق', 's06.title': 'الخطوات<br>القادمة',
        'road.f1.title': 'نظام الفأرة', 'road.f1.desc': 'الكشف عن المواضع والنقرات وأحداث hover في الطرفية.',
        'road.f2.title': 'واجهة الصوت', 'road.f2.desc': 'مؤثرات صوتية وإشعارات صوتية مدمجة في محرك الواجهات.',
        'road.f3.title': 'مشاريع نموذجية', 'road.f3.desc': 'محاكاة الكسورية، لعبة T-Rex، واجهة مصغرة كاملة.',
        'road.f4.title': 'مستودع قالب', 'road.f4.desc': 'مستودع جاهز لبدء مشروع Terminal-Renderer خلال دقائق.',
        'road.f5.title': 'توثيق شامل', 'road.f5.desc': 'توثيق Doxygen الداخلي + Markdown مفصّل لكل وحدة.',
        'road.f6.title': 'تحليل الذاكرة', 'road.f6.desc': 'نظام مدمج لتحليل الذاكرة ووحدة المعالجة.',
        's06.p1': 'الرؤية طويلة المدى هي أن يصبح محرك عرض وحدة طرفية مفتوح المصدر، معياريًا وقابلًا للتوسع.',
        's06.p2': 'هدف الأداء ثابت: تشغيل المحرك حتى على أضعف الأجهزة. كل تحسين يُحسب، كل تخصيص يُقاس.',
        's06.p3': 'وحدة <code>ContentReorganisation()</code> قيد التحسين لتقليل إعادة الحسابات غير الضرورية.',
        'bottom.doc': 'التوثيق', 'bottom.about': 'عني',
        'doc.hero.eyebrow': 'الأدوات والمرجع',
        'doc.hero.sub': 'استورد وعاين وعدّل وأنشئ ملفات .widg مباشرة في المتصفح',
        'doc.s01.num': '٠١ / معاينة', 'doc.s01.title': 'استيراد &amp;<br>تحرير ملف .widg',
        'doc.s02.num': '٠٢ / أداة الإنشاء', 'doc.s02.title': 'إنشاء ملف .widg<br>من الصفر',
        'about.eyebrow': 'عني', 'about.sub': 'مطور C++ · منشئ Terminal-Renderer',
        'about.bio1': 'شغوف بالأنظمة ذات المستوى المنخفض وعرض الطرفية وهندسة البرمجيات.',
        'about.bio2': 'كل البنية التحتية للمحرك مصممة بدون تبعيات رسومية خارجية.',
        'about.bio3': 'مكتوب بـ C++20/C++23، مُرمَّز بـ UTF-8، يستهدف التوافق مع Windows وLinux.',
        'about.quote': '"اعمل مع القيود، لا رغمها."',
        'about.card.tech': 'المكدس التقني', 'about.card.project': 'المشروع',
        'about.card.timeline': 'الجدول الزمني',
        'about.github': 'GitHub — RydeProgramation ↗', 'about.contact': 'تواصل',
        'about.scroll': 'مرر',
    }
};

/* ══════════════════════════════════════
   i18n ENGINE
   FR = HTML d'origine (sauvegardé avant toute traduction).
   On ne touche jamais T pour le FR : il suffit d'écrire
   le texte français directement dans les fichiers .html.
══════════════════════════════════════ */
let currentLang = 'fr';
const originalHTML = new Map(); // élément -> innerHTML français d'origine

function captureOriginals() {
    document.querySelectorAll('[data-i18n]').forEach(el => {
        if (!originalHTML.has(el)) originalHTML.set(el, el.innerHTML);
    });
}

window.setLang = function(lang) {
    if (lang !== 'fr' && !T[lang]) lang = 'fr';
    currentLang = lang;

    if (lang === 'fr') {
        // Restaure le HTML français tel qu'écrit dans la page
        document.querySelectorAll('[data-i18n]').forEach(el => {
            if (originalHTML.has(el)) el.innerHTML = originalHTML.get(el);
        });
    } else {
        const t = T[lang];
        document.querySelectorAll('[data-i18n]').forEach(el => {
            const key = el.dataset.i18n;
            if (t[key] != null) el.innerHTML = t[key];
        });
    }

    document.documentElement.dir  = lang === 'ar' ? 'rtl' : 'ltr';
    document.documentElement.lang = lang;
    document.querySelectorAll('.lang-btn').forEach(b => {
        b.classList.toggle('active', b.dataset.lang === lang);
    });
    try { localStorage.setItem('tr-lang', lang); } catch(e) {}
};

document.addEventListener('DOMContentLoaded', () => {
    captureOriginals(); // toujours capturer le FR AVANT la première traduction

    document.querySelectorAll('.lang-btn').forEach(b => {
        b.addEventListener('click', () => window.setLang(b.dataset.lang));
    });
    let saved = null;
    try { saved = localStorage.getItem('tr-lang'); } catch(e) {}
    window.setLang(saved || 'fr');
});

})();