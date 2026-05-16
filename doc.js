/* ══════════════════════════════════════════════
   doc.js — Terminal-Renderer Documentation Tool
   Logique complète : Typewriter, Parser .widg,
   Générateur, Prévisualiseur, Créateur
══════════════════════════════════════════════ */

/* ══════════════════════════════════════════════
   TYPEWRITER — "Documentation"
══════════════════════════════════════════════ */
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

/* ══════════════════════════════════════════════
   COLOR UTILITIES
══════════════════════════════════════════════ */
function parseColor(hex) {
    if (!hex) return { hex6: '#ffffff', alpha: 255 };
    const h = hex.replace('#', '');
    return {
        hex6:  '#' + h.substr(0, 6),
        alpha: h.length >= 8 ? parseInt(h.substr(6, 2), 16) : 255
    };
}

function toCSS(hex) {
    const { hex6, alpha } = parseColor(hex);
    const h = hex6.replace('#', '');
    const r = parseInt(h.substr(0,2),16);
    const g = parseInt(h.substr(2,2),16);
    const b = parseInt(h.substr(4,2),16);
    return `rgba(${r},${g},${b},${(alpha/255).toFixed(2)})`;
}

function toHexAA(hex6, alphaInt) {
    const a = Math.round(Math.max(0, Math.min(255, alphaInt))).toString(16).padStart(2,'0');
    return hex6 + a;
}

function pct(a) { return Math.round(a / 255 * 100) + '%'; }

function bindAlpha(rangeId, valId) {
    const r = document.getElementById(rangeId);
    const v = document.getElementById(valId);
    if (!r || !v) return;
    r.addEventListener('input', () => v.textContent = pct(r.value));
}
bindAlpha('edit-fg-a','edit-fg-av');
bindAlpha('edit-bg-a','edit-bg-av');
bindAlpha('cr-fg-a','cr-fg-av');
bindAlpha('cr-bg-a','cr-bg-av');

/* ══════════════════════════════════════════════
   .WIDG PARSER
══════════════════════════════════════════════ */
function parseWidg(xmlStr) {
    const parser = new DOMParser();
    const doc = parser.parseFromString(xmlStr, 'text/xml');
    if (doc.querySelector('parsererror')) {
        const msg = doc.querySelector('parsererror').textContent;
        throw new Error('XML invalide : ' + msg.split('\n')[0]);
    }

    const root = doc.querySelector('trObject');
    if (!root) throw new Error('Élément <trObject> introuvable dans le fichier');

    const type = root.getAttribute('type') || 'trWidget';
    const name = root.getAttribute('name') || 'Widget';

    // Properties
    const properties = Array.from(doc.querySelectorAll('Properties > Property')).map(p => ({
        name:  p.getAttribute('name')  || '',
        ptype: p.getAttribute('type')  || 'bool',
        value: p.textContent.trim()    || 'true'
    }));
    if (!properties.length) {
        properties.push(
            { name:'Activate', ptype:'bool', value:'true'  },
            { name:'Protected',ptype:'bool', value:'false' },
            { name:'ToChange', ptype:'bool', value:'true'  }
        );
    }

    // Position
    const posEl = root.querySelector('trPawn > Position');
    const relEl = root.querySelector('trPawn > RelativePositionType');
    const position = {
        x:      posEl ? posEl.getAttribute('x')        : '0',
        y:      posEl ? posEl.getAttribute('y')        : '0',
        rpType: relEl ? relEl.getAttribute('RpType')   : 'MiddleCenter'
    };

    // Find direct child trWidget or trText
    let widgetEl = null, textEl = null;
    for (const child of root.children) {
        if (child.tagName === 'trWidget') widgetEl = child;
        if (child.tagName === 'trText')   textEl   = child;
    }

    function extractWidget(el) {
        const sizeEl  = el.querySelector(':scope > Size');
        const colorEl = el.querySelector(':scope > Color');
        const lines   = Array.from(el.querySelectorAll(':scope > Content > Line'))
                            .map(l => l.getAttribute('Content') || '');

        // Extract CaseColor intervals
        const cases = Array.from(el.querySelectorAll(':scope > CaseColor > Case')).map(c => ({
            start: parseInt(c.getAttribute('Start') || '0'),
            end:   parseInt(c.getAttribute('End') || '0'),
            fg:    c.getAttribute('foreground') || null,
            bg:    c.getAttribute('background') || null,  // sans underscore = actif
        })).filter(c => c.end > c.start);

        return {
            width:  sizeEl  ? (parseInt(sizeEl.getAttribute('width'))  || 20) : 20,
            height: sizeEl  ? (parseInt(sizeEl.getAttribute('height')) || 5)  : 5,
            fg: colorEl ? colorEl.getAttribute('foreground') : '#ffffffff',
            bg: colorEl ? colorEl.getAttribute('background') : '#000000ff',
            lines: lines.length ? lines : [''],
            cases: cases.length ? cases : null
        };
    }

    let widget    = widgetEl ? extractWidget(widgetEl) : (textEl ? extractWidget(textEl) : null);
    let animation = null;

    if (textEl) {
        const frames = [];
        textEl.querySelectorAll(':scope > Animation > RawFrame').forEach(rf => {
            const fLines = Array.from(rf.querySelectorAll('Content > Line'))
                               .map(l => l.getAttribute('Content') || '');
            // Extract per-frame CaseColor
            const fCases = Array.from(rf.querySelectorAll('CaseColor > Case')).map(c => ({
                start: parseInt(c.getAttribute('Start') || '0'),
                end:   parseInt(c.getAttribute('End') || '0'),
                fg:    c.getAttribute('foreground') || null,
                bg:    c.getAttribute('background') || null,
            })).filter(c => c.end > c.start);

            frames.push({
                number: parseInt(rf.getAttribute('number') || '0'),
                time:   parseInt(rf.getAttribute('time')   || '200'),
                lines:  fLines.length ? fLines : [''],
                cases:  fCases.length ? fCases : null
            });
        });
        frames.sort((a, b) => a.number - b.number);
        if (frames.length) animation = frames;
    }

    if (!widget) widget = { width:20, height:5, fg:'#ffffffff', bg:'#000000ff', lines:[''], cases:null };

    return { type, name, properties, position, widget, animation, isAnimated: !!animation };
}

/* ══════════════════════════════════════════════
   .WIDG GENERATOR
══════════════════════════════════════════════ */
function generateWidg(data) {
    const e = s => String(s||'')
        .replace(/&/g,'&amp;').replace(/</g,'&lt;')
        .replace(/>/g,'&gt;').replace(/"/g,'&quot;');

    const props = (data.properties || []).map(p =>
        `        <Property name="${e(p.name)}" type="${e(p.ptype||'bool')}">${e(p.value)}</Property>`
    ).join('\n');

    const w = data.widget || { width:20, height:5, fg:'#ffffffff', bg:'#000000ff', lines:[''] };
    const lines = (w.lines||['']).map(l => `            <Line Content="${e(l)}" />`).join('\n');

    const tag = data.type === 'trText' ? 'trText' : 'trWidget';
    const pos = data.position || { x:0, y:0, rpType:'MiddleCenter' };

    let animBlock = '';
    if (data.type === 'trText' && data.animation?.length) {
        const frames = data.animation.map((f, i) => {
            const fl = (f.lines||['']).map(l => `                    <Line Content="${e(l)}" />`).join('\n');
            return `            <RawFrame number="${i}" time="${f.time||200}">\n                <Content>\n${fl}\n                </Content>\n            </RawFrame>`;
        }).join('\n');
        animBlock = `\n        <Animation>\n${frames}\n        </Animation>`;
    }

    return `<?xml version="1.0" encoding="utf-8"?>\n<trObject type="${e(data.type)}" name="${e(data.name)}">\n    <Properties>\n${props}\n    </Properties>\n    <trActor>\n        <APPLY_Implementation Code="" />\n    </trActor>\n    <trPawn>\n        <Position x="${e(String(pos.x))}" y="${e(String(pos.y))}" />\n        <RelativePositionType RpType="${e(pos.rpType)}" />\n    </trPawn>\n    <${tag}>\n        <Size width="${w.width}" height="${w.height}" />\n        <Content>\n${lines}\n        </Content>\n        <Color foreground="${e(w.fg)}" background="${e(w.bg)}" />${animBlock}\n    </${tag}>\n</trObject>`;
}

/* ══════════════════════════════════════════════
   DOWNLOAD HELPER
══════════════════════════════════════════════ */
function download(content, filename) {
    const blob = new Blob([content], { type: 'text/xml;charset=utf-8' });
    const url  = URL.createObjectURL(blob);
    const a    = Object.assign(document.createElement('a'), { href: url, download: filename + '.widg' });
    document.body.appendChild(a); a.click();
    document.body.removeChild(a); URL.revokeObjectURL(url);
}

/* ══════════════════════════════════════════════
   RENDER PREVIEW  (supports CaseColor intervals)
══════════════════════════════════════════════ */
function renderPreview(widget, lines, targetId, cases) {
    const el = document.getElementById(targetId);
    if (!el) return;

    const text = (lines || widget.lines || ['']).join('\n');
    const activeCases = cases || widget.cases;

    el.style.backgroundColor = toCSS(widget.bg || '#000000ff');
    el.style.color            = toCSS(widget.fg || '#ffffffff');

    if (!activeCases || !activeCases.length) {
        el.textContent = text;
        return;
    }

    // Build colored spans from CaseColor intervals
    el.innerHTML = '';
    let pos = 0;
    // Sort cases by start
    const sorted = [...activeCases].sort((a,b) => a.start - b.start);

    for (const c of sorted) {
        if (pos < c.start) {
            el.appendChild(document.createTextNode(text.slice(pos, c.start)));
        }
        if (c.start < c.end && c.start < text.length) {
            const span = document.createElement('span');
            span.textContent = text.slice(c.start, Math.min(c.end, text.length));
            if (c.fg) span.style.color           = toCSS(c.fg);
            if (c.bg) span.style.backgroundColor = toCSS(c.bg);
            el.appendChild(span);
            pos = c.end;
        }
    }
    if (pos < text.length) {
        el.appendChild(document.createTextNode(text.slice(pos)));
    }
}

/* ══════════════════════════════════════════════
   VIEWER / EDITOR
══════════════════════════════════════════════ */
let ST       = null;
let animTmr  = null;
let curFrame = 0;

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
    if (ST.widget?.cases) badge('🎨 CaseColor', 'anim');

    document.getElementById('edit-name').value   = ST.name;
    document.getElementById('edit-width').value  = ST.widget?.width  || 20;
    document.getElementById('edit-height').value = ST.widget?.height || 5;
    if (ST.widget) {
        const fg = parseColor(ST.widget.fg);
        const bg = parseColor(ST.widget.bg);
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
    else               { ap.classList.remove('on'); }

    updatePreview();
}

function updatePreview() {
    if (!ST?.widget) return;
    const frame = (ST.isAnimated && ST.animation?.[curFrame]) ? ST.animation[curFrame] : null;
    const lines = frame ? frame.lines : ST.widget.lines;
    const cases = frame ? (frame.cases || ST.widget.cases) : ST.widget.cases;
    renderPreview(ST.widget, lines, 'terminal-content', cases);
}

function updateFrameInd() {
    const total = ST?.animation?.length || 1;
    document.getElementById('frame-ind').textContent = `Frame ${curFrame + 1} / ${total}`;
}

function stopAnim() {
    if (animTmr) { clearInterval(animTmr); animTmr = null; }
    document.getElementById('btn-play')?.classList.remove('active');
}

function rebuildLines() {
    const c = document.getElementById('lines-editor');
    c.innerHTML = '';
    (ST?.widget?.lines || []).forEach((ln, i) => {
        c.appendChild(makeLineRow(ln, i, (v) => {
            ST.widget.lines[i] = v;
            updatePreview();
        }, () => {
            ST.widget.lines.splice(i, 1);
            rebuildLines(); updatePreview();
        }));
    });
}

function makeLineRow(text, idx, onChange, onDel) {
    const row = document.createElement('div');
    row.className = 'line-row';
    const num = document.createElement('span');
    num.className = 'line-num'; num.textContent = idx + 1;
    const inp = document.createElement('input');
    inp.type = 'text'; inp.className = 'line-input'; inp.value = text;
    inp.addEventListener('input', () => onChange(inp.value));
    const del = document.createElement('button');
    del.className = 'line-del'; del.textContent = '✕';
    del.addEventListener('click', onDel);
    row.appendChild(num); row.appendChild(inp); row.appendChild(del);
    return row;
}

// Editor live-update
['edit-name','edit-width','edit-height','edit-fg','edit-fg-a','edit-bg','edit-bg-a'].forEach(id => {
    const el = document.getElementById(id);
    if (!el) return;
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
    if (!ST?.widget) return;
    ST.widget.lines.push(''); rebuildLines(); updatePreview();
});

// Animation buttons
document.getElementById('btn-prev')?.addEventListener('click', () => {
    stopAnim();
    if (!ST?.animation?.length) return;
    curFrame = (curFrame - 1 + ST.animation.length) % ST.animation.length;
    updateFrameInd(); updatePreview();
});
document.getElementById('btn-next')?.addEventListener('click', () => {
    stopAnim();
    if (!ST?.animation?.length) return;
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

// Download / Reset
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

// Drop zone / file input
function handleFile(file) {
    if (!file) return;
    const reader = new FileReader();
    reader.onload = e => {
        const errEl = document.getElementById('parse-error');
        try {
            const parsed = parseWidg(e.target.result);
            errEl.style.display = 'none';
            loadState(parsed, file.name);
        } catch (err) {
            errEl.textContent = '✗ ' + err.message;
            errEl.style.display = 'block';
        }
    };
    reader.readAsText(file);
}

const dz = document.getElementById('drop-zone');
const fi = document.getElementById('file-input');
if (dz && fi) {
    dz.addEventListener('dragover',  e => { e.preventDefault(); dz.classList.add('drag-over'); });
    dz.addEventListener('dragleave', () => dz.classList.remove('drag-over'));
    dz.addEventListener('drop', e => { e.preventDefault(); dz.classList.remove('drag-over'); handleFile(e.dataTransfer.files[0]); });
    dz.addEventListener('click', () => fi.click());
    fi.addEventListener('change', e => handleFile(e.target.files[0]));
}

/* ══════════════════════════════════════════════
   CREATOR
══════════════════════════════════════════════ */
const CR = {
    name: 'MyWidget', type: 'trWidget',
    position: { x: 0, y: 0, rpType: 'MiddleCenter' },
    widget: { width: 20, height: 3, fg: '#ffffffff', bg: '#000000ff', lines: [''] },
    animation: [],
    isAnimated: false,
    properties: [
        { name:'Activate',  ptype:'bool', value:'true'  },
        { name:'Protected', ptype:'bool', value:'false' },
        { name:'ToChange',  ptype:'bool', value:'true'  }
    ]
};

function crRebuildLines() {
    const c = document.getElementById('cr-lines-editor');
    if (!c) return;
    c.innerHTML = '';
    CR.widget.lines.forEach((ln, i) => {
        c.appendChild(makeLineRow(ln, i, v => {
            CR.widget.lines[i] = v; crUpdatePreview();
        }, () => {
            CR.widget.lines.splice(i, 1); crRebuildLines(); crUpdatePreview();
        }));
    });
}

function crRebuildFrames() {
    const c = document.getElementById('cr-frame-list');
    if (!c) return;
    c.innerHTML = '';
    CR.animation.forEach((fr, fi) => {
        const item = document.createElement('div');
        item.className = 'frame-item';

        const head = document.createElement('div');
        head.className = 'frame-item-head';

        const title = document.createElement('span');
        title.className = 'frame-item-title';
        title.textContent = `RawFrame ${fi}`;

        const tw = document.createElement('div');
        tw.className = 'frame-time-wrap';
        tw.innerHTML = '<span class="frame-time-label">Durée (ms)</span>';
        const ti = document.createElement('input');
        ti.type = 'number'; ti.className = 'frame-time-input'; ti.value = fr.time;
        ti.addEventListener('input', () => { CR.animation[fi].time = parseInt(ti.value)||200; });
        tw.appendChild(ti);

        const del = document.createElement('button');
        del.className = 'frame-del'; del.textContent = '✕';
        del.addEventListener('click', () => { CR.animation.splice(fi,1); crRebuildFrames(); });

        head.appendChild(title); head.appendChild(tw); head.appendChild(del);

        const linesC = document.createElement('div');
        (fr.lines||['']).forEach((ln, li) => {
            linesC.appendChild(makeLineRow(ln, li, v => {
                CR.animation[fi].lines[li] = v;
            }, () => {
                CR.animation[fi].lines.splice(li,1); crRebuildFrames();
            }));
        });

        const addLn = document.createElement('button');
        addLn.className = 'doc-btn doc-btn-ghost doc-btn-sm';
        addLn.textContent = '+ Ligne';
        addLn.addEventListener('click', () => { CR.animation[fi].lines.push(''); crRebuildFrames(); });

        item.appendChild(head); item.appendChild(linesC); item.appendChild(addLn);
        c.appendChild(item);
    });
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
    const animWrap = document.getElementById('cr-anim-wrap');
    if (animWrap) animWrap.style.display = CR.isAnimated ? 'block' : 'none';
}

function crUpdatePreview() {
    crSyncFromForm();
    renderPreview(CR.widget, null, 'cr-terminal-content', null);
    const nameEl = document.getElementById('cr-preview-name');
    if (nameEl) nameEl.textContent = CR.name;
    const xmlEl = document.getElementById('cr-xml-preview');
    if (xmlEl) xmlEl.textContent = generateWidg(CR);
}

// Bind all creator inputs
['cr-name','cr-type','cr-rp','cr-x','cr-y','cr-w','cr-h',
 'cr-fg','cr-fg-a','cr-bg','cr-bg-a'].forEach(id => {
    const el = document.getElementById(id);
    if (!el) return;
    el.addEventListener('input', crUpdatePreview);
    el.addEventListener('change', crUpdatePreview);
});

document.getElementById('cr-add-line')?.addEventListener('click', () => {
    CR.widget.lines.push(''); crRebuildLines(); crUpdatePreview();
});
document.getElementById('cr-add-frame')?.addEventListener('click', () => {
    CR.animation.push({ number: CR.animation.length, time: 200, lines: [''] });
    crRebuildFrames();
});
document.getElementById('cr-download')?.addEventListener('click', () => {
    crSyncFromForm();
    download(generateWidg(CR), CR.name || 'widget');
    const s = document.getElementById('cr-success');
    if (s) { s.style.display = 'block'; setTimeout(() => s.style.display='none', 3000); }
});

// Init creator
crRebuildLines();
crUpdatePreview();
