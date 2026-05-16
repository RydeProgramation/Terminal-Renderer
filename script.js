(() => {

    /* ══════════════════════════════════════
       ASCII CANVAS
    ══════════════════════════════════════ */
    const canvas = document.getElementById('ascii-canvas');
    const ctx    = canvas.getContext('2d');

    const CHARS_RICH  = '░▒▓│┤╡╢╖╕╣║╗╝╜╛┐└┴┬├─┼╞╟╚╔╩╦╠═╬╧╨╤╥╙╘╒╓╫╪┘┌█▄▌▐▀αßΓπΣσµτΦΘΩδ∞φε∩≡±≥≤⌠⌡÷≈°∙·√ⁿ²';
    const CHARS_PUNCT = '.,\';:!?/\\|[]{}()<>@#$%^&*-_=+~`"';
    const ALL_CHARS   = CHARS_RICH + CHARS_PUNCT;
    const MAX_COLS    = 160;
    const MAX_ROWS    = 90;

    let W, H, cols, rows, grid, ages, effectiveFW, effectiveFH;
    let mouseX = -999, mouseY = -999, hasMoused = false;
    let t = 0;

    function randomChar() {
        return ALL_CHARS[Math.floor(Math.random() * ALL_CHARS.length)];
    }

    function resize() {
        W = window.innerWidth;
        H = window.innerHeight;
        canvas.width  = W;
        canvas.height = H;
        cols = Math.min(Math.ceil(W / 7), MAX_COLS);
        rows = Math.min(Math.ceil(H / 13), MAX_ROWS);
        effectiveFW = W / cols;
        effectiveFH = H / rows;
        grid = [];
        ages = [];
        for (let r = 0; r < rows; r++) {
            grid[r] = [];
            ages[r]  = [];
            for (let c = 0; c < cols; c++) {
                grid[r][c] = randomChar();
                ages[r][c]  = Math.random() * 60;
            }
        }
    }

    /* ── Mouse / Touch tracking ─────────── */
    window.addEventListener('mousemove', e => {
        mouseX = e.clientX; mouseY = e.clientY; hasMoused = true;
    });
    window.addEventListener('mouseleave', () => { hasMoused = false; });

    window.addEventListener('touchmove', e => {
        mouseX = e.touches[0].clientX;
        mouseY = e.touches[0].clientY;
        hasMoused = true;
    }, { passive: true });
    window.addEventListener('touchend', () => { hasMoused = false; });

    /* ── Noise function ─────────────────── */
    function noise(x, y, t) {
        return (
            Math.sin(x * 0.13 + t * 0.7) * Math.cos(y * 0.11 - t * 0.5) +
            Math.sin((x + y) * 0.08 + t * 0.3) +
            Math.sin(x * 0.05 - t * 0.2) * Math.cos(y * 0.17 + t * 0.4)
        );
    }

    /* ── Render loop ────────────────────── */
    function draw() {
        t += 0.016;
        ctx.clearRect(0, 0, W, H);

        const fontSize = Math.max(7, Math.floor(effectiveFH * 0.82));
        ctx.font = `${fontSize}px 'IBM Plex Mono','Courier New',monospace`;
        ctx.textBaseline = 'top';

        const mc = mouseX / effectiveFW;
        const mr = mouseY / effectiveFH;

        for (let r = 0; r < rows; r++) {
            for (let c = 0; c < cols; c++) {
                const n         = noise(c, r, t);
                const intensity = (n + 2) / 4;

                const distCenter = Math.hypot(c - cols / 2, r - rows / 2);
                const centerPull = Math.max(0, 1 - distCenter / (Math.min(cols, rows) * 0.42));

                let mousePull = 0;
                if (hasMoused) {
                    const dm = Math.hypot(c - mc, r - mr);
                    mousePull = Math.max(0, 1 - dm / 10);
                }

                // Age & character refresh
                ages[r][c] += 0.35 + n * 0.25 + mousePull * 1.8 + centerPull * 0.4;
                const thresh = 18 + Math.sin(c * 0.2 + r * 0.15 + t) * 8;
                if (ages[r][c] > thresh) {
                    ages[r][c]  = 0;
                    grid[r][c] = randomChar();
                }

                // Alpha — mouse boost kept subtle
                const alpha = Math.max(
                    0.025,
                    Math.min(0.55, intensity * 0.27 + mousePull * 0.12 + centerPull * 0.20)
                );

                // Color
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


    /* ══════════════════════════════════════
       TYPEWRITER — Hero title
    ══════════════════════════════════════ */
    const elTerminal = document.getElementById('title-terminal');
    const elRenderer = document.getElementById('title-renderer');
    const elCursor   = document.getElementById('title-cursor');

    const fullText = 'Terminal-Renderer';
    const splitAt  = 'Terminal-'.length; // index where "Renderer" starts
    let   idx      = 0;
    const speed    = 80; // ms per character

    function typeNext() {
        if (idx <= splitAt) {
            elTerminal.textContent = fullText.slice(0, idx);
        } else {
            elTerminal.textContent = fullText.slice(0, splitAt);
            elRenderer.textContent = fullText.slice(splitAt, idx);
        }
        idx++;

        if (idx <= fullText.length) {
            setTimeout(typeNext, speed);
        } else {
            // Done typing — activate shimmer on "Renderer", hide cursor
            setTimeout(() => {
                elRenderer.classList.add('shimmer-text');
                elCursor.style.display = 'none';
            }, 400);
        }
    }

    // Start after the hero card fade-in animation
    setTimeout(typeNext, 900);


    /* ══════════════════════════════════════
       3D TILT — Module cards
    ══════════════════════════════════════ */
    const MAX_TILT  = 14; // degrees max
    const PERSP     = 600; // px

    document.querySelectorAll('.module-item').forEach(card => {
        card.addEventListener('mousemove', e => {
            const rect = card.getBoundingClientRect();
            const dx   = (e.clientX - rect.left - rect.width  / 2) / (rect.width  / 2); // -1 → +1
            const dy   = (e.clientY - rect.top  - rect.height / 2) / (rect.height / 2); // -1 → +1

            card.style.transform  = `perspective(${PERSP}px) rotateX(${-dy * MAX_TILT}deg) rotateY(${dx * MAX_TILT}deg) translateZ(10px)`;
            card.style.transition = 'transform 0.08s ease, border-color 0.3s, box-shadow 0.3s';

            // Shine position in %
            const mx = ((e.clientX - rect.left) / rect.width  * 100).toFixed(1) + '%';
            const my = ((e.clientY - rect.top)  / rect.height * 100).toFixed(1) + '%';
            card.style.setProperty('--mx', mx);
            card.style.setProperty('--my', my);
        });

        card.addEventListener('mouseleave', () => {
            card.style.transform  = `perspective(${PERSP}px) rotateX(0deg) rotateY(0deg) translateZ(0px)`;
            card.style.transition = 'transform 0.5s ease, border-color 0.3s, box-shadow 0.3s';
        });
    });

})();