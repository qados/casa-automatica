#include <SPI.h>
#include <Ethernet.h>

void gerarSite(EthernetClient client) {
	client.println(F("<!DOCTYPE html> \
<html lang='pt-BR'> \
<head> \
<meta charset='UTF-8'> \
<meta name='viewport' content='width=device-width, initial-scale=1'> \
<meta name='theme-color' content='#071426'> \
<title>Casa Automatizada</title> \
<!-- <link rel='stylesheet' href='style.css'> --> \
 \
</head> \
<body> \
<header> \
  <img src='imagem_trabalho.jpeg' alt='logo' width='100'> \
  <div class='logo' aria-hidden='true'></div> \
  <div><h1>Casa Automatizada</h1><p>Controle sua casa de forma simples e segura</p></div> \
  <div class='status'><span class='dot'></span> Demonstração </div> \
</header> \
<main> \
  <div class='notice'><strong>teste:</strong>TESTE N 90 </div> \
 \
  <section class='section'> \
    <div class='section-title'><h2> Iluminação dos cômodos</h2><span>Até 3 cômodos diferentes por vez</span></div> \
    <div class='rooms' id='rooms'></div> \
  </section> \
 \
  <div class='grid2'> \
    <section class='section panel'> \
      <div class='section-title'><h2> Janelas</h2><span>Abrir ou fechar</span></div> \
      <div class='cards' id='windows'></div> \
    </section> \
    <section class='section panel'> \
      <div class='section-title'><h2> Portões</h2><span>Garagem e entrada</span></div> \
      <div class='cards' id='gates'></div> \
    </section> \
  </div> \
 \
  <div class='bottom section'> \
    <section class='panel'> \
      <div class='section-title'><h2> Regras </h2></div> \
      <ul class='rule'> \
        <li>Apenas um usuário pode controlar pelo site por vez.</li> \
        <li>Cada sessão pode selecionar até 3 cômodos diferentes.</li> \
        <li>Ao selecionar um quarto cômodo, a sessão termina automaticamente.</li> \
        <li>Após terminar, o mesmo usuário fica bloqueado por 15 minutos.</li> \
        <li>Outro usuário pode assumir o controle quando a sessão estiver livre.</li> \
      </ul> \
    </section> \
    <section class='panel'> \
      <div class='section-title'><h2> Sua sessão</h2></div> \
      <div class='session-stat'><span>Cômodos selecionados</span><strong id='count'>0/3</strong></div> \
      <button class='blue' id='reset'>Reiniciar </button> \
    </section> \
  </div> \
</main> \
<script> \
const roomNames = [ \
    'Sala / térreo', \
    'Quarto 1', \
    'Quarto 2', \
    'Cozinha', \
    'Banheiro' \
]; \
 \
const roomIcons = ['🛋️', '🛏️', '🛌', '🍽️', '🚿']; \
 \
const roomState = roomNames.map(() => ({ \
    on: false, \
    used: false \
})); \
 \
const devices = { \
    windows: [ \
        { name: 'Janela 1', icon: '🪟', open: false }, \
        { name: 'Janela 2', icon: '🪟', open: false } \
    ], \
 \
    gates: [ \
        { name: 'Portão da garagem', icon: '🚘', open: false }, \
        { name: 'Portão da casa', icon: '🚪', open: false } \
    ] \
}; \
 \
let locked = false; \
 \
function el(tag, cls, text) { \
    const n = document.createElement(tag); \
 \
    if (cls) { \
        n.className = cls; \
    } \
 \
    if (text !== undefined) { \
        n.textContent = text; \
    } \
 \
    return n; \
} \
 \
function renderRooms() { \
    const root = document.getElementById('rooms'); \
    root.replaceChildren(); \
 \
    roomNames.forEach((name, i) => { \
        const r = roomState[i]; \
 \
        const card = el('article', 'card'); \
        const art = el('div', 'room-art', roomIcons[i]); \
        const h = el('h3', '', name); \
 \
        const state = el( \
            'p', \
            'state' + (r.on ? ' on' : ''), \
            r.on ? '● Luz ligada' : '● Luz desligada' \
        ); \
 \
        const use = el( \
            'button', \
            'primary', \
            r.on ? 'Desligar luz' : 'Ligar luz' \
        ); \
 \
        const usedRooms = roomState.filter(x => x.used).length; \
 \
        use.disabled = locked || (!r.used && usedRooms >= 3); \
 \
        use.onclick = () => { \
            if (!r.used) { \
                r.used = true; \
            } \
 \
            r.on = !r.on; \
 \
            renderRooms(); \
            updateSession(); \
        }; \
 \
        card.append(art, h, state, use); \
        root.append(card); \
    }); \
} \
 \
function renderDevices(key, targetId) { \
    const root = document.getElementById(targetId); \
    root.replaceChildren(); \
 \
    devices[key].forEach(d => { \
        const card = el('article', 'card'); \
        const row = el('div', 'device'); \
        const ico = el('div', 'device-icon', d.icon); \
        const info = el('div', 'device-info'); \
 \
        const state = el( \
            'p', \
            'state' + (d.open ? ' on' : ''), \
            d.open ? '● Aberto' : '● Fechado' \
        ); \
 \
        info.append( \
            el('h3', '', d.name), \
            state \
        ); \
 \
        const actions = el('div', 'device-actions'); \
 \
        ['Abrir', 'Fechar'].forEach((label, j) => { \
            const b = el( \
                'button', \
                j === 0 ? 'primary' : 'secondary', \
                label \
            ); \
 \
            b.disabled = locked; \
 \
            b.onclick = () => { \
                d.open = j === 0; \
                renderDevices(key, targetId); \
            }; \
 \
            actions.append(b); \
        }); \
 \
        row.append(ico, info); \
        card.append(row, actions); \
        root.append(card); \
    }); \
} \
 \
function updateSession() { \
    const count = roomState.filter(r => r.used).length; \
 \
    document.getElementById('count').textContent = count + '/3'; \
 \
    if (count >= 3) { \
        locked = true; \
 \
        renderRooms(); \
        renderDevices('windows', 'windows'); \
        renderDevices('gates', 'gates'); \
 \
        document.getElementById('sessionState').textContent = \
            'Limite atingido'; \
    } \
} \
 \
document.getElementById('reset').onclick = () => { \
    roomState.forEach(r => { \
        r.on = false; \
        r.used = false; \
    }); \
 \
    Object.values(devices).flat().forEach(d => { \
        d.open = false; \
    }); \
 \
    locked = false; \
 \
    renderRooms(); \
    renderDevices('windows', 'windows'); \
    renderDevices('gates', 'gates'); \
    updateSession(); \
}; \
 \
renderRooms(); \
renderDevices('windows', 'windows'); \
renderDevices('gates', 'gates'); \
updateSession(); \
 \
</script> \
 \
<style> \
    :root { \
    --bg: #06111f; \
    --panel: #0b1c30; \
    --card: #10253d; \
    --line: #1d3a59; \
    --text: #eef6ff; \
    --muted: #9db2cb; \
    --blue: #1688ff; \
    --green: #00bd91; \
    --red: #ff4d67; \
    --yellow: #ffc857; \
} \
 \
* { \
    box-sizing: border-box; \
} \
 \
body { \
    margin: 0; \
    background: linear-gradient(145deg, #06111f, #09182a 60%, #06111f); \
    color: var(--text); \
    font: 15px system-ui, -apple-system, 'Segoe UI', Roboto, Arial, sans-serif; \
} \
 \
header { \
    padding: 22px clamp(16px, 4vw, 42px); \
    border-bottom: 1px solid var(--line); \
    display: flex; \
    align-items: center; \
    gap: 14px; \
    flex-wrap: wrap; \
} \
 \
.logo { \
    font-size: 34px; \
} \
 \
h1 { \
    font-size: clamp(21px, 3vw, 30px); \
    margin: 0; \
} \
 \
header p { \
    margin: 4px 0 0; \
    color: var(--muted); \
} \
 \
.status { \
    margin-left: auto; \
    display: flex; \
    gap: 8px; \
    align-items: center; \
    color: #5ce3bd; \
    font-weight: 700; \
} \
 \
.dot { \
    width: 9px; \
    height: 9px; \
    background: var(--green); \
    border-radius: 50%; \
    display: inline-block; \
} \
 \
main { \
    max-width: 1400px; \
    margin: auto; \
    padding: 24px clamp(14px, 3vw, 32px) 40px; \
} \
 \
.notice { \
    background: #332913; \
    border: 1px solid #7c5c22; \
    color: #ffe4a3; \
    padding: 13px 16px; \
    border-radius: 12px; \
    margin-bottom: 22px; \
    font-size: 13px; \
} \
 \
.section { \
    margin: 22px 0; \
} \
 \
.section-title { \
    display: flex; \
    align-items: center; \
    gap: 10px; \
    margin-bottom: 14px; \
} \
 \
.section-title h2 { \
    font-size: 20px; \
    margin: 0; \
} \
 \
.section-title span { \
    color: var(--muted); \
    font-size: 13px; \
} \
 \
.rooms { \
    display: grid; \
    grid-template-columns: repeat(5, minmax(0, 1fr)); \
    gap: 13px; \
} \
 \
.grid2 { \
    display: grid; \
    grid-template-columns: repeat(2, minmax(0, 1fr)); \
    gap: 16px; \
} \
 \
.cards { \
    display: grid; \
    grid-template-columns: repeat(2, minmax(0, 1fr)); \
    gap: 12px; \
} \
 \
.card, \
.panel { \
    background: linear-gradient(150deg, #10253d, #0a1b2e); \
    border: 1px solid var(--line); \
    border-radius: 16px; \
    padding: 16px; \
    min-width: 0; \
    box-shadow: 0 8px 24px #0002; \
} \
 \
.room-art { \
    height: 82px; \
    border-radius: 11px; \
    background: linear-gradient(135deg, #183858, #0a1728); \
    display: grid; \
    place-items: center; \
    font-size: 42px; \
    margin-bottom: 12px; \
} \
 \
.card h3 { \
    margin: 0 0 8px; \
    font-size: 16px; \
} \
 \
.state { \
    color: var(--muted); \
    font-size: 13px; \
    margin: 0 0 14px; \
    display: flex; \
    align-items: center; \
    gap: 7px; \
} \
 \
.state .dot { \
    background: #73869c; \
} \
 \
.state.on { \
    color: #ffe3a0; \
} \
 \
.state.on .dot { \
    background: var(--yellow); \
} \
 \
button { \
    border: 1px solid transparent; \
    border-radius: 10px; \
    padding: 11px 12px; \
    font: inherit; \
    font-weight: 700; \
    cursor: pointer; \
    transition: 0.15s; \
    width: 100%; \
    margin-top: 7px; \
} \
 \
button:focus-visible { \
    outline: 3px solid #70b8ff; \
    outline-offset: 2px; \
} \
 \
.primary { \
    background: var(--green); \
    color: #03261f; \
} \
 \
.secondary { \
    background: #203751; \
    color: #dcecff; \
    border-color: #2a4969; \
} \
 \
.blue { \
    background: #087be5; \
    color: white; \
} \
 \
.danger { \
    background: #482131; \
    color: #ffd9df; \
    border-color: #783246; \
} \
 \
button:disabled { \
    opacity: 0.35; \
    cursor: not-allowed; \
} \
 \
.device { \
    display: flex; \
    gap: 13px; \
    align-items: center; \
} \
 \
.device-icon { \
    width: 68px; \
    height: 76px; \
    flex: none; \
    display: grid; \
    place-items: center; \
    border-radius: 10px; \
    background: #172f4b; \
    font-size: 34px; \
} \
 \
.device-info { \
    flex: 1; \
    min-width: 0; \
} \
 \
.device-info h3 { \
    font-size: 15px; \
} \
</style> \
</body> \
</html>"));
};
