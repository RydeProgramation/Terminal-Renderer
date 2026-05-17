/* ══════════════════════════════════════════════
   doc.js — Terminal-Renderer Documentation Tool
══════════════════════════════════════════════ */

/* ══ TYPEWRITER ══ */
(function () {
    const elMain   = document.getElementById('title-main');
    const elCursor = document.getElementById('title-cursor');
    if (!elMain) return;
    const word = 'Documentation';
    let i = 0;
    function type() {
        elMain.textContent = word.slice(0, i++);
        if (i <= word.length) setTimeout(type, 80);
        else setTimeout(() => {
            elMain.classList.add('shimmer-text');
            if (elCursor) elCursor.style.display = 'none';
        }, 400);
    }
    setTimeout(type, 900);
})();

/* ══ COLOR UTILITIES ══ */
function parseColor(hex) {
    if (!hex) return { hex6: '#ffffff', alpha: 255 };
    const h = hex.replace('#', '');
    return {
        hex6:  '#' + h.substr(0, 6),
        alpha: h.length >= 8 ? parseInt(h.substr(6, 2), 16) : 255
    };
}
function toCSS(hex) {
    if (!hex) return 'transparent';
    const { hex6, alpha } = parseColor(hex);
    const h = hex6.replace('#', '');
    const r = parseInt(h.substr(0,2),16), g = parseInt(h.substr(2,2),16), b = parseInt(h.substr(4,2),16);
    return `rgba(${r},${g},${b},${(alpha/255).toFixed(2)})`;
}
function toHexAA(hex6, alphaInt) {
    const a = Math.round(Math.max(0, Math.min(255, alphaInt))).toString(16).padStart(2,'0');
    return hex6 + a;
}
function pct(a) { return Math.round(a / 255 * 100) + '%'; }
function bindAlpha(rangeId, valId) {
    const r = document.getElementById(rangeId), v = document.getElementById(valId);
    if (!r || !v) return;
    r.addEventListener('input', () => v.textContent = pct(r.value));
}
bindAlpha('edit-fg-a','edit-fg-av'); bindAlpha('edit-bg-a','edit-bg-av');
bindAlpha('cr-fg-a','cr-fg-av');    bindAlpha('cr-bg-a','cr-bg-av');

/* ══ XML ESCAPE PROCESSING
   Mirrors C++ trLoadFile behavior:
   \b → backspace (0x08), \n → newline, \t → tab, \f \v \r aussi.
   Les attributs "Line Content" dans le XML contiennent des séquences
   littérales type \b que le C++ convertit à la fermeture de </trWidget>.
══ */
function processXmlEscapes(s) {
    let out = '';
    for (let i = 0; i < s.length; i++) {
        if (s[i] === '\\' && i + 1 < s.length) {
            switch (s[i+1]) {
                case 'b':  out += '\b'; i++; break;
                case 'n':  out += '\n'; i++; break;
                case 't':  out += '\t'; i++; break;
                case 'f':  out += '\f'; i++; break;
                case 'v':  out += '\v'; i++; break;
                case 'r':  out += '\r'; i++; break;
                case '\\': out += '\\'; i++; break;
                default:   out += s[i];
            }
        } else { out += s[i]; }
    }
    return out;
}

/* ══ .WIDG PARSER ══ */
function parseCases(el) {
    return Array.from(el.querySelectorAll(':scope > CaseColor > Case')).map(c => ({
        start: parseInt(c.getAttribute('Start') || c.getAttribute('start') || '0'),
        end:   parseInt(c.getAttribute('End')   || c.getAttribute('end')   || '0'),
        fg:    c.getAttribute('foreground') || null,
        // N'activer bg que si l'attribut exact "background" existe (sans préfixe _)
        bg:    c.getAttribute('background') || null,
    })).filter(c => c.end > c.start);
}

function parseWidg(xmlStr) {
    const parser = new DOMParser();
    const doc = parser.parseFromString(xmlStr, 'text/xml');
    if (doc.querySelector('parsererror')) {
        throw new Error('XML invalide : ' + doc.querySelector('parsererror').textContent.split('\n')[0]);
    }
    const root = doc.querySelector('trObject');
    if (!root) throw new Error('Élément <trObject> introuvable dans le fichier');

    const type = root.getAttribute('type') || 'trWidget';
    const name = root.getAttribute('name') || 'Widget';

    const properties = Array.from(doc.querySelectorAll('Properties > Property')).map(p => ({
        name: p.getAttribute('name') || '', ptype: p.getAttribute('type') || 'bool',
        value: p.textContent.trim() || 'true'
    }));
    if (!properties.length) properties.push(
        { name:'Activate', ptype:'bool', value:'true' },
        { name:'Protected', ptype:'bool', value:'false' },
        { name:'ToChange', ptype:'bool', value:'true' }
    );

    const posEl = root.querySelector('trPawn > Position');
    const relEl = root.querySelector('trPawn > RelativePositionType');
    const position = {
        x:      posEl ? posEl.getAttribute('x')      : '0',
        y:      posEl ? posEl.getAttribute('y')      : '0',
        rpType: relEl ? relEl.getAttribute('RpType') : 'MiddleCenter'
    };

    let widgetEl = null, textEl = null;
    for (const ch of root.children) {
        if (ch.tagName === 'trWidget') widgetEl = ch;
        if (ch.tagName === 'trText')   textEl   = ch;
    }

    function extractWidget(el) {
        const sizeEl  = el.querySelector(':scope > Size');
        const colorEl = el.querySelector(':scope > Color');
        // Appliquer processXmlEscapes : convertit \b, \n, etc. comme le C++
        const lines = Array.from(el.querySelectorAll(':scope > Content > Line'))
            .map(l => processXmlEscapes(l.getAttribute('Content') || ''));
        const cases = parseCases(el);
        return {
            width:  sizeEl ? (parseInt(sizeEl.getAttribute('width'))  || 20) : 20,
            height: sizeEl ? (parseInt(sizeEl.getAttribute('height')) || 5)  : 5,
            fg: colorEl ? colorEl.getAttribute('foreground') : '#ffffffff',
            bg: colorEl ? colorEl.getAttribute('background') : '#000000ff',
            lines: lines.length ? lines : [''],
            cases: cases.length ? cases : null
        };
    }

    let widget = widgetEl ? extractWidget(widgetEl) : (textEl ? extractWidget(textEl) : null);
    let animation = null;

    if (textEl) {
        const frames = [];
        textEl.querySelectorAll(':scope > Animation > RawFrame').forEach(rf => {
            const fLines = Array.from(rf.querySelectorAll('Content > Line'))
                .map(l => processXmlEscapes(l.getAttribute('Content') || ''));
            frames.push({
                number: parseInt(rf.getAttribute('number') || '0'),
                time:   parseInt(rf.getAttribute('time')   || '200'),
                lines:  fLines.length ? fLines : [''],
                cases:  parseCases(rf) || null
            });
        });
        frames.sort((a, b) => a.number - b.number);
        if (frames.length) animation = frames;
    }

    if (!widget) widget = { width:20, height:5, fg:'#ffffffff', bg:'#000000ff', lines:[''], cases:null };
    return { type, name, properties, position, widget, animation, isAnimated: !!animation };
}

/* ══ .WIDG GENERATOR ══ */
function generateWidg(data) {
    const e = s => String(s||'')
        .replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;').replace(/"/g,'&quot;');

    const props = (data.properties || []).map(p =>
        `        <Property name="${e(p.name)}" type="${e(p.ptype||'bool')}">${e(p.value)}</Property>`
    ).join('\n');

    const w = data.widget || { width:20, height:5, fg:'#ffffffff', bg:'#000000ff', lines:[''] };
    const lines = (w.lines||['']).map(l => `            <Line Content="${e(l)}" />`).join('\n');

    let caseColorBlock = '';
    if (w.cases && w.cases.length) {
        const cl = w.cases.map(c => {
            let attrs = `Start="${c.start}" End="${c.end}"`;
            if (c.fg) attrs += ` foreground="${e(c.fg)}"`;
            attrs += c.bg ? ` background="${e(c.bg)}"` : ` _background="#ffffffff"`;
            return `            <Case ${attrs}/>`;
        }).join('\n');
        caseColorBlock = `\n        <CaseColor>\n${cl}\n        </CaseColor>`;
    }

    const tag = data.type === 'trText' ? 'trText' : 'trWidget';
    const pos = data.position || { x:0, y:0, rpType:'MiddleCenter' };

    let animBlock = '';
    if (data.type === 'trText' && data.animation?.length) {
        const frs = data.animation.map((f, i) => {
            const fl = (f.lines||['']).map(l => `                    <Line Content="${e(l)}" />`).join('\n');
            return `            <RawFrame number="${i}" time="${f.time||200}">\n                <Content>\n${fl}\n                </Content>\n            </RawFrame>`;
        }).join('\n');
        animBlock = `\n        <Animation>\n${frs}\n        </Animation>`;
    }

    return `<?xml version="1.0" encoding="utf-8"?>
<trObject type="${e(data.type)}" name="${e(data.name)}">
    <Properties>
${props}
    </Properties>
    <trActor>
        <APPLY_Implementation Code="" />
    </trActor>
    <trPawn>
        <Position x="${e(String(pos.x))}" y="${e(String(pos.y))}" />
        <RelativePositionType RpType="${e(pos.rpType)}" />
    </trPawn>
    <${tag}>
        <Size width="${w.width}" height="${w.height}" />
        <Content>
${lines}
        </Content>
        <Color foreground="${e(w.fg)}" background="${e(w.bg)}" />${caseColorBlock}${animBlock}
    </${tag}>
</trObject>`;
}

/* ══ DOWNLOAD ══ */
function download(content, filename) {
    const blob = new Blob([content], { type: 'text/xml;charset=utf-8' });
    const url  = URL.createObjectURL(blob);
    const a    = Object.assign(document.createElement('a'), { href: url, download: filename + '.widg' });
    document.body.appendChild(a); a.click();
    document.body.removeChild(a); URL.revokeObjectURL(url);
}

/* ══ GRID RENDERER
   Simule le comportement de ContentReorganisationKeepColor() du C++ :
   - Construit une grille width × height de caractères
   - Gère \b (backspace → recule d'une case et efface), \n, \r, \t
   - Applique CaseColor sur les positions RAW → positions grille
   - Les espaces "vides" conservent bien la couleur de fond de la Case
══ */
function renderPreview(widget, lines, targetId, cases) {
    const el = document.getElementById(targetId);
    if (!el) return;

    const rawLines   = lines || widget.lines || [''];
    const W          = Math.max(1, widget.width  || 20);
    const H          = Math.max(1, widget.height || 5);
    const fgCSS      = toCSS(widget.fg || '#ffffffff');
    const bgCSS      = toCSS(widget.bg || '#000000ff');
    const activeCases = cases || widget.cases || null;

    // Jointure des lignes avec \n — le flux est ensuite traité comme dans le C++
    const rawText = rawLines.join('\n');

    // ── Construire la grille de caractères ──
    const grid = Array.from({ length: H }, () => new Array(W).fill(' '));

    // posMap[i] = position {row, col} dans la grille du caractère i du rawText
    // (position où le caractère a été ÉCRIT, avant tout décalage ultérieur)
    const posMap = new Array(rawText.length).fill(null);

    let row = 0, col = 0;
    for (let i = 0; i < rawText.length && row < H; i++) {
        const code = rawText.charCodeAt(i);
        const ch   = rawText[i];

        if (code === 0x08 /* backspace \b */) {
            // Recule d'une position et efface la case
            if (col > 0) {
                col--;
            } else if (row > 0) {
                row--; col = W - 1;
            }
            grid[row][col] = ' ';
            posMap[i] = { row, col }; // position effacée

        } else if (ch === '\n' || ch === '\f' || ch === '\v') {
            posMap[i] = { row, col };
            col = 0; row++;

        } else if (ch === '\r') {
            posMap[i] = { row, col };
            col = 0;

        } else if (ch === '\t') {
            posMap[i] = { row, col };
            // Prochain multiple de 4
            const stop = Math.min(Math.floor(col / 4 + 1) * 4, W);
            while (col < stop) { grid[row][col] = ' '; col++; }
            if (col >= W) { col = 0; row++; }

        } else {
            // Retour à la ligne automatique si dépassement de la largeur
            if (col >= W) { col = 0; row++; }
            if (row >= H) break;
            posMap[i] = { row, col };
            grid[row][col] = ch;
            col++;
        }
    }

    // ── Construire la grille de couleurs ──
    // colorGrid[r][c] = { fg: cssString|null, bg: cssString|null }
    // Initialiser avec null partout (pas de surcharge → hérité du conteneur)
    const colorGrid = Array.from({ length: H }, () =>
        Array.from({ length: W }, () => ({ fg: null, bg: null }))
    );

    if (activeCases && activeCases.length) {
        // Trier par start pour que les cases ultérieures puissent surcharger
        const sorted = [...activeCases].sort((a, b) => a.start - b.start);

        for (const c of sorted) {
            const fgVal = c.fg ? toCSS(c.fg) : null;
            const bgVal = c.bg ? toCSS(c.bg) : null;

            for (let rawPos = c.start; rawPos < c.end; rawPos++) {
                const mapped = posMap[rawPos];
                if (!mapped) continue;
                const { row: r, col: cl } = mapped;
                if (r >= H || cl >= W) continue;
                // Appliquer fg et/ou bg (bg null si attribut _background désactivé)
                if (fgVal) colorGrid[r][cl].fg = fgVal;
                if (bgVal) colorGrid[r][cl].bg = bgVal;
            }

            // ── FIX CRITIQUE : les espaces dans l'intervalle doivent aussi
            //    avoir la bonne couleur, même s'il n'y a pas de caractère mappé.
            //    On balaye la grille pour les positions non mappées entre start/end.
            //    Pour cela on cherche la plage de cellules couverte.
            if (bgVal) {
                // Trouver min/max positions mappées pour cette case
                let minR = H, minC = W, maxR = -1, maxC = -1;
                for (let rawPos = c.start; rawPos < c.end; rawPos++) {
                    const m = posMap[rawPos];
                    if (!m) continue;
                    if (m.row < minR || (m.row === minR && m.col < minC)) { minR = m.row; minC = m.col; }
                    if (m.row > maxR || (m.row === maxR && m.col > maxC)) { maxR = m.row; maxC = m.col; }
                }
                // Colorier toutes les cellules du rectangle row entre minR..maxR
                if (maxR >= 0) {
                    for (let r = minR; r <= maxR && r < H; r++) {
                        const cStart = (r === minR) ? minC : 0;
                        const cEnd   = (r === maxR) ? maxC + 1 : W;
                        for (let cl = cStart; cl < cEnd; cl++) {
                            if (bgVal) colorGrid[r][cl].bg = bgVal;
                            if (fgVal && !colorGrid[r][cl].fg) colorGrid[r][cl].fg = fgVal;
                        }
                    }
                }
            }
        }
    }

    // ── Rendu DOM ──
    el.style.backgroundColor = bgCSS;
    el.style.color = fgCSS;
    el.innerHTML = '';

    for (let r = 0; r < H; r++) {
        const lineDiv = document.createElement('div');
        lineDiv.style.cssText = 'white-space:pre;display:block;min-height:1em;';

        let s = 0;
        while (s < W) {
            const cc = colorGrid[r][s];
            // Étendre le span tant que même fg+bg
            let end = s + 1;
            while (end < W && colorGrid[r][end].fg === cc.fg && colorGrid[r][end].bg === cc.bg) end++;

            const span = document.createElement('span');
            span.textContent = grid[r].slice(s, end).join('');
            if (cc.fg) span.style.color = cc.fg;
            if (cc.bg) span.style.backgroundColor = cc.bg;
            lineDiv.appendChild(span);
            s = end;
        }
        el.appendChild(lineDiv);
    }
}

/* ══ VIEWER / EDITOR ══ */
let ST = null, animTmr = null, curFrame = 0;

function loadState(parsed, filename) {
    ST = JSON.parse(JSON.stringify(parsed));
    curFrame = 0; stopAnim();

    document.getElementById('drop-zone').style.display = 'none';
    document.getElementById('viewer').classList.add('active');
    document.getElementById('preview-filename').textContent = filename || 'widget.widg';

    const meta = document.getElementById('preview-meta');
    meta.innerHTML = '';
    const badge = (txt, cls) => {
        const b = document.createElement('span');
        b.className = 'preview-badge' + (cls ? ' '+cls : '');
        b.textContent = txt; meta.appendChild(b);
    };
    badge(ST.type); badge(ST.name);
    if (ST.widget) badge(`${ST.widget.width}×${ST.widget.height}`);
    if (ST.isAnimated) badge('🎞 Animé', 'anim');
    if (ST.widget?.cases?.length) badge('🎨 CaseColor', 'anim');

    document.getElementById('edit-name').value   = ST.name;
    document.getElementById('edit-width').value  = ST.widget?.width  || 20;
    document.getElementById('edit-height').value = ST.widget?.height || 5;
    if (ST.widget) {
        const fg = parseColor(ST.widget.fg), bg = parseColor(ST.widget.bg);
        document.getElementById('edit-fg').value = fg.hex6;
        document.getElementById('edit-fg-a').value = fg.alpha;
        document.getElementById('edit-fg-av').textContent = pct(fg.alpha);
        document.getElementById('edit-bg').value = bg.hex6;
        document.getElementById('edit-bg-a').value = bg.alpha;
        document.getElementById('edit-bg-av').textContent = pct(bg.alpha);
    }
    rebuildLines();
    const ap = document.getElementById('anim-panel');
    if (ST.isAnimated) { ap.classList.add('on'); updateFrameInd(); }
    else ap.classList.remove('on');
    updatePreview();
}

function updatePreview() {
    if (!ST?.widget) return;
    const frame = (ST.isAnimated && ST.animation?.[curFrame]) ? ST.animation[curFrame] : null;
    const lines = frame ? frame.lines : ST.widget.lines;
    const cases = frame ? (frame.cases?.length ? frame.cases : ST.widget.cases) : ST.widget.cases;
    renderPreview(ST.widget, lines, 'terminal-content', cases);
}

function updateFrameInd() {
    const total = ST?.animation?.length || 1;
    const el = document.getElementById('frame-ind');
    if (el) el.textContent = `Frame ${curFrame + 1} / ${total}`;
}

function stopAnim() {
    if (animTmr) { clearTimeout(animTmr); animTmr = null; }
    document.getElementById('btn-play')?.classList.remove('active');
}

function rebuildLines() {
    const c = document.getElementById('lines-editor');
    if (!c) return;
    c.innerHTML = '';
    (ST?.widget?.lines || []).forEach((ln, i) => {
        c.appendChild(makeLineRow(ln, i, v => { ST.widget.lines[i] = v; updatePreview(); },
            () => { ST.widget.lines.splice(i, 1); rebuildLines(); updatePreview(); }));
    });
}

function makeLineRow(text, idx, onChange, onDel) {
    const row = document.createElement('div'); row.className = 'line-row';
    const num = document.createElement('span'); num.className = 'line-num'; num.textContent = idx + 1;
    const inp = document.createElement('input'); inp.type = 'text'; inp.className = 'line-input'; inp.value = text;
    inp.addEventListener('input', () => onChange(inp.value));
    const del = document.createElement('button'); del.className = 'line-del'; del.textContent = '✕';
    del.addEventListener('click', onDel);
    row.appendChild(num); row.appendChild(inp); row.appendChild(del);
    return row;
}

['edit-name','edit-width','edit-height','edit-fg','edit-fg-a','edit-bg','edit-bg-a'].forEach(id => {
    const el = document.getElementById(id); if (!el) return;
    el.addEventListener('input', () => {
        if (!ST) return;
        ST.name = document.getElementById('edit-name').value;
        if (ST.widget) {
            ST.widget.width  = parseInt(document.getElementById('edit-width').value)  || 20;
            ST.widget.height = parseInt(document.getElementById('edit-height').value) || 5;
            ST.widget.fg = toHexAA(document.getElementById('edit-fg').value, +document.getElementById('edit-fg-a').value);
            ST.widget.bg = toHexAA(document.getElementById('edit-bg').value, +document.getElementById('edit-bg-a').value);
        }
        updatePreview();
    });
});

document.getElementById('btn-add-line')?.addEventListener('click', () => {
    if (!ST?.widget) return; ST.widget.lines.push(''); rebuildLines(); updatePreview();
});
document.getElementById('btn-prev')?.addEventListener('click', () => {
    stopAnim(); if (!ST?.animation?.length) return;
    curFrame = (curFrame - 1 + ST.animation.length) % ST.animation.length;
    updateFrameInd(); updatePreview();
});
document.getElementById('btn-next')?.addEventListener('click', () => {
    stopAnim(); if (!ST?.animation?.length) return;
    curFrame = (curFrame + 1) % ST.animation.length;
    updateFrameInd(); updatePreview();
});
document.getElementById('btn-play')?.addEventListener('click', () => {
    if (animTmr) { stopAnim(); return; }
    if (!ST?.animation?.length) return;
    document.getElementById('btn-play').classList.add('active');
    const tick = () => {
        curFrame = (curFrame + 1) % ST.animation.length;
        updateFrameInd(); updatePreview();
        animTmr = setTimeout(tick, ST.animation[curFrame]?.time || 200);
    };
    animTmr = setTimeout(tick, ST.animation[curFrame]?.time || 200);
});
document.getElementById('btn-stop')?.addEventListener('click', stopAnim);
document.getElementById('btn-download')?.addEventListener('click', () => {
    if (!ST) return;
    download(generateWidg(ST), ST.name || 'widget');
    const s = document.getElementById('dl-success');
    if (s) { s.style.display = 'block'; setTimeout(() => s.style.display='none', 3000); }
});
document.getElementById('btn-reset')?.addEventListener('click', () => {
    ST = null; curFrame = 0; stopAnim();
    document.getElementById('drop-zone').style.display = '';
    document.getElementById('viewer').classList.remove('active');
    document.getElementById('file-input').value = '';
    document.getElementById('parse-error').style.display = 'none';
});

function handleFile(file) {
    if (!file) return;
    const reader = new FileReader();
    reader.onload = ev => {
        const errEl = document.getElementById('parse-error');
        try {
            const parsed = parseWidg(ev.target.result);
            errEl.style.display = 'none';
            loadState(parsed, file.name);
        } catch (err) {
            errEl.textContent = '✗ ' + err.message;
            errEl.style.display = 'block';
        }
    };
    reader.readAsText(file);
}
const dz = document.getElementById('drop-zone'), fi = document.getElementById('file-input');
if (dz && fi) {
    dz.addEventListener('dragover',  e => { e.preventDefault(); dz.classList.add('drag-over'); });
    dz.addEventListener('dragleave', () => dz.classList.remove('drag-over'));
    dz.addEventListener('drop', e => { e.preventDefault(); dz.classList.remove('drag-over'); handleFile(e.dataTransfer.files[0]); });
    dz.addEventListener('click', () => fi.click());
    fi.addEventListener('change', e => handleFile(e.target.files[0]));
}

/* ══ CREATOR ══ */
const CR = {
    name: 'MyWidget', type: 'trWidget',
    position: { x: 0, y: 0, rpType: 'MiddleCenter' },
    widget: { width: 20, height: 3, fg: '#ffffffff', bg: '#000000ff', lines: [''], cases: [] },
    animation: [], isAnimated: false,
    properties: [
        { name:'Activate',  ptype:'bool', value:'true' },
        { name:'Protected', ptype:'bool', value:'false' },
        { name:'ToChange',  ptype:'bool', value:'true' }
    ]
};

let crCurFrame = 0, crAnimTmr = null;

function crStopAnim() {
    if (crAnimTmr) { clearTimeout(crAnimTmr); crAnimTmr = null; }
    document.getElementById('cr-btn-play')?.classList.remove('active');
}

function crUpdateFrameInd() {
    const total = CR.animation.length || 1;
    const el = document.getElementById('cr-frame-ind');
    if (el) el.textContent = `Frame ${crCurFrame + 1} / ${Math.max(1, CR.animation.length)}`;
}

function crRebuildLines() {
    const c = document.getElementById('cr-lines-editor'); if (!c) return;
    c.innerHTML = '';
    CR.widget.lines.forEach((ln, i) => {
        c.appendChild(makeLineRow(ln, i,
            v => { CR.widget.lines[i] = v; crUpdatePreview(); },
            () => { CR.widget.lines.splice(i, 1); crRebuildLines(); crUpdatePreview(); }
        ));
    });
}

function crRebuildCases() {
    const c = document.getElementById('cr-cases-editor'); if (!c) return;
    c.innerHTML = '';

    CR.widget.cases.forEach((cs, i) => {
        const row = document.createElement('div');
        row.style.cssText = 'display:grid;grid-template-columns:60px 60px 1fr 1fr auto;gap:6px;align-items:center;margin-bottom:8px;padding:10px;border:0.5px solid rgba(255,255,255,0.06);border-radius:1px;background:rgba(255,255,255,0.01);';

        // Start / End
        const mkNumInp = (val, label, onChange) => {
            const w = document.createElement('div');
            w.style.cssText = 'display:flex;flex-direction:column;gap:3px;';
            const lbl = document.createElement('span');
            lbl.style.cssText = 'font-size:9px;letter-spacing:0.1em;color:rgba(255,255,255,0.25);';
            lbl.textContent = label;
            const inp = document.createElement('input');
            inp.type = 'number'; inp.className = 'size-input'; inp.value = val; inp.min = 0;
            inp.style.cssText = 'font-size:11px;padding:4px;';
            inp.addEventListener('input', () => onChange(parseInt(inp.value)||0));
            w.appendChild(lbl); w.appendChild(inp); return w;
        };
        row.appendChild(mkNumInp(cs.start, 'START', v => { CR.widget.cases[i].start = v; crUpdatePreview(); }));
        row.appendChild(mkNumInp(cs.end,   'END',   v => { CR.widget.cases[i].end   = v; crUpdatePreview(); }));

        // FG picker
        const fgW = document.createElement('div');
        fgW.style.cssText = 'display:flex;flex-direction:column;gap:3px;';
        const fgLbl = document.createElement('span');
        fgLbl.style.cssText = 'font-size:9px;letter-spacing:0.1em;color:rgba(255,255,255,0.25);';
        fgLbl.textContent = 'FG COLOR';
        const fgPick = document.createElement('input'); fgPick.type = 'color';
        fgPick.value = cs.fg ? '#' + cs.fg.replace('#','').substr(0,6) : '#ffffff';
        fgPick.style.cssText = 'width:100%;height:28px;cursor:pointer;border:0.5px solid rgba(255,255,255,0.15);border-radius:1px;background:none;';
        fgPick.addEventListener('input', () => { CR.widget.cases[i].fg = fgPick.value + 'ff'; crUpdatePreview(); });
        fgW.appendChild(fgLbl); fgW.appendChild(fgPick); row.appendChild(fgW);

        // BG picker + toggle
        const bgW = document.createElement('div');
        bgW.style.cssText = 'display:flex;flex-direction:column;gap:3px;';
        const bgHead = document.createElement('div');
        bgHead.style.cssText = 'display:flex;align-items:center;gap:5px;';
        const bgToggle = document.createElement('input'); bgToggle.type = 'checkbox';
        bgToggle.checked = !!cs.bg; bgToggle.title = 'Activer BG';
        bgToggle.style.cssText = 'cursor:pointer;accent-color:rgba(180,200,255,0.8);';
        const bgLbl = document.createElement('span');
        bgLbl.style.cssText = 'font-size:9px;letter-spacing:0.1em;color:rgba(255,255,255,0.25);';
        bgLbl.textContent = 'BG COLOR';
        bgHead.appendChild(bgToggle); bgHead.appendChild(bgLbl);
        const bgPick = document.createElement('input'); bgPick.type = 'color';
        bgPick.value = cs.bg ? '#' + cs.bg.replace('#','').substr(0,6) : '#000000';
        bgPick.disabled = !cs.bg;
        bgPick.style.cssText = `width:100%;height:28px;cursor:pointer;border:0.5px solid rgba(255,255,255,0.15);border-radius:1px;background:none;opacity:${cs.bg ? 1 : 0.3};transition:opacity 0.2s;`;
        bgToggle.addEventListener('change', () => {
            bgPick.disabled = !bgToggle.checked;
            bgPick.style.opacity = bgToggle.checked ? '1' : '0.3';
            CR.widget.cases[i].bg = bgToggle.checked ? bgPick.value + 'ff' : null;
            crUpdatePreview();
        });
        bgPick.addEventListener('input', () => {
            if (bgToggle.checked) { CR.widget.cases[i].bg = bgPick.value + 'ff'; crUpdatePreview(); }
        });
        bgW.appendChild(bgHead); bgW.appendChild(bgPick); row.appendChild(bgW);

        // Delete
        const del = document.createElement('button'); del.className = 'line-del'; del.textContent = '✕';
        del.addEventListener('click', () => { CR.widget.cases.splice(i, 1); crRebuildCases(); crUpdatePreview(); });
        row.appendChild(del);

        c.appendChild(row);
    });
}

function crRebuildFrames() {
    const c = document.getElementById('cr-frame-list'); if (!c) return;
    c.innerHTML = '';
    CR.animation.forEach((fr, fi) => {
        const item = document.createElement('div'); item.className = 'frame-item';
        const head = document.createElement('div'); head.className = 'frame-item-head';
        const title = document.createElement('span'); title.className = 'frame-item-title';
        title.textContent = `RawFrame ${fi}`;
        const tw = document.createElement('div'); tw.className = 'frame-time-wrap';
        tw.innerHTML = '<span class="frame-time-label">Durée (ms)</span>';
        const ti = document.createElement('input');
        ti.type = 'number'; ti.className = 'frame-time-input'; ti.value = fr.time;
        ti.addEventListener('input', () => { CR.animation[fi].time = parseInt(ti.value)||200; });
        tw.appendChild(ti);
        const del = document.createElement('button'); del.className = 'frame-del'; del.textContent = '✕';
        del.addEventListener('click', () => {
            CR.animation.splice(fi, 1);
            if (crCurFrame >= CR.animation.length) crCurFrame = Math.max(0, CR.animation.length - 1);
            crRebuildFrames(); crUpdatePreview();
        });
        head.appendChild(title); head.appendChild(tw); head.appendChild(del);
        const linesC = document.createElement('div');
        (fr.lines||['']).forEach((ln, li) => {
            linesC.appendChild(makeLineRow(ln, li,
                v => { CR.animation[fi].lines[li] = v; crUpdatePreview(); },
                () => { CR.animation[fi].lines.splice(li,1); crRebuildFrames(); crUpdatePreview(); }
            ));
        });
        const addLn = document.createElement('button');
        addLn.className = 'doc-btn doc-btn-ghost doc-btn-sm'; addLn.textContent = '+ Ligne';
        addLn.addEventListener('click', () => { CR.animation[fi].lines.push(''); crRebuildFrames(); crUpdatePreview(); });
        item.appendChild(head); item.appendChild(linesC); item.appendChild(addLn);
        c.appendChild(item);
    });
    crUpdateFrameInd();
}

function crSyncFromForm() {
    CR.name = document.getElementById('cr-name')?.value || 'MyWidget';
    CR.type = document.getElementById('cr-type')?.value || 'trWidget';
    CR.position = {
        x: document.getElementById('cr-x')?.value || '0',
        y: document.getElementById('cr-y')?.value || '0',
        rpType: document.getElementById('cr-rp')?.value || 'MiddleCenter'
    };
    CR.widget.width  = parseInt(document.getElementById('cr-w')?.value)  || 20;
    CR.widget.height = parseInt(document.getElementById('cr-h')?.value)  || 3;
    CR.widget.fg = toHexAA(document.getElementById('cr-fg')?.value || '#ffffff', +(document.getElementById('cr-fg-a')?.value || 255));
    CR.widget.bg = toHexAA(document.getElementById('cr-bg')?.value || '#000000', +(document.getElementById('cr-bg-a')?.value || 255));
    CR.isAnimated = CR.type === 'trText';
    const animWrap  = document.getElementById('cr-anim-wrap');
    const animPanel = document.getElementById('cr-anim-panel');
    if (animWrap)  animWrap.style.display  = CR.isAnimated ? 'block' : 'none';
    if (animPanel) animPanel.style.display = (CR.isAnimated && CR.animation.length > 0) ? 'block' : 'none';
}

function crUpdatePreview() {
    crSyncFromForm();
    // Choisir ce qu'on prévisualise
    let previewLines = CR.widget.lines;
    let previewCases = CR.widget.cases.length ? CR.widget.cases : null;
    if (CR.isAnimated && CR.animation.length > 0) {
        const frame = CR.animation[crCurFrame];
        if (frame) { previewLines = frame.lines; previewCases = null; }
    }
    renderPreview(CR.widget, previewLines, 'cr-terminal-content', previewCases);
    const nameEl = document.getElementById('cr-preview-name');
    if (nameEl) nameEl.textContent = CR.name;
    const xmlEl = document.getElementById('cr-xml-preview');
    if (xmlEl) xmlEl.textContent = generateWidg(CR);
    crUpdateFrameInd();
}

// Binder les inputs du créateur
['cr-name','cr-type','cr-rp','cr-x','cr-y','cr-w','cr-h','cr-fg','cr-fg-a','cr-bg','cr-bg-a'].forEach(id => {
    const el = document.getElementById(id); if (!el) return;
    el.addEventListener('input', crUpdatePreview);
    el.addEventListener('change', crUpdatePreview);
});

document.getElementById('cr-add-line')?.addEventListener('click', () => {
    CR.widget.lines.push(''); crRebuildLines(); crUpdatePreview();
});
document.getElementById('cr-add-case')?.addEventListener('click', () => {
    CR.widget.cases.push({ start: 0, end: 5, fg: '#ffffffff', bg: null });
    crRebuildCases(); crUpdatePreview();
});
document.getElementById('cr-add-frame')?.addEventListener('click', () => {
    CR.animation.push({ number: CR.animation.length, time: 200, lines: [''] });
    crCurFrame = CR.animation.length - 1;
    crRebuildFrames(); crUpdatePreview();
});

// Contrôles animation du créateur
document.getElementById('cr-btn-prev')?.addEventListener('click', () => {
    crStopAnim(); if (!CR.animation.length) return;
    crCurFrame = (crCurFrame - 1 + CR.animation.length) % CR.animation.length;
    crUpdatePreview();
});
document.getElementById('cr-btn-next')?.addEventListener('click', () => {
    crStopAnim(); if (!CR.animation.length) return;
    crCurFrame = (crCurFrame + 1) % CR.animation.length;
    crUpdatePreview();
});
document.getElementById('cr-btn-play')?.addEventListener('click', () => {
    if (crAnimTmr) { crStopAnim(); return; }
    if (!CR.animation.length) return;
    document.getElementById('cr-btn-play')?.classList.add('active');
    const tick = () => {
        crCurFrame = (crCurFrame + 1) % CR.animation.length;
        crUpdatePreview();
        crAnimTmr = setTimeout(tick, CR.animation[crCurFrame]?.time || 200);
    };
    crAnimTmr = setTimeout(tick, CR.animation[crCurFrame]?.time || 200);
});
document.getElementById('cr-btn-stop')?.addEventListener('click', crStopAnim);

document.getElementById('cr-download')?.addEventListener('click', () => {
    crSyncFromForm();
    download(generateWidg(CR), CR.name || 'widget');
    const s = document.getElementById('cr-success');
    if (s) { s.style.display = 'block'; setTimeout(() => s.style.display='none', 3000); }
});

// Init
crRebuildLines();
crRebuildCases();
crUpdatePreview();
