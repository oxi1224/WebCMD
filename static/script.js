import { genContent, genWindow, fetchOrigin } from "./util.js";

let CUR_TERM = 0;

const newWindowBtn = document.getElementById("new-terminal");
const openWindowsElm = document.querySelector(".open-windows");
const contentElm = document.querySelector(".content");
const terminalIndicatorElm = document.querySelector(".terminal-indicator");
const inputForm = document.getElementById("input-form");
const cmdInput = document.getElementById("cmd-input");

window.addEventListener("load", async () => {
  const data = await fetchOrigin("/api/getTerminalIDs", {
    method: "GET",
    cache: "no-cache"
  }).then(async res => await res.json());

  for (const terminalID of data) {
    openWindowsElm.appendChild(genWindow(terminalID));
  }

  const windowElm = openWindowsElm.querySelector(".window");
  if (windowElm) {
    const terminalNum = parseInt(windowElm.getAttribute("data-terminal-id"));
    await selectTerminal(terminalNum);
  } else {
    const terminalNum = await fetchOrigin("/api/createTerminal", {
      method: "POST",
      cache: "no-cache"
    }).then(async res => res.text());
    await selectTerminal(parseInt(terminalNum), true);
  }

  [...document.querySelectorAll(".window")].forEach(windowElm => {
    windowElm.addEventListener("click", async () => {
      await selectTerminal(parseInt(windowElm.getAttribute("data-terminal-id")), false);
    });
  
    const closeBtn = windowElm.querySelector("button");
    closeBtn.addEventListener("click", async () => {
      await fetchOrigin("/api/closeTerminal", {
        method: "POST",
        body: windowElm.getAttribute("data-terminal-id"),
        headers: {
          "Content-Type": "text/plain"
        }
      });
      windowElm.remove();
    });
  });
});

inputForm.addEventListener("submit", async (e) => {
  e.preventDefault();
  await exec(cmdInput.value);
  cmdInput.value = "";
});

newWindowBtn.addEventListener("click", async () => {
  const terminalNum = await fetchOrigin("/api/createTerminal", {
    method: "POST",
    cache: "no-cache"
  }).then(async res => res.text());
  await selectTerminal(parseInt(terminalNum), true);
});

/**
 * @param {number} num 
 * @param {boolean} addNewWindowElm
 */
async function selectTerminal(num, addNewWindowElm) {
  /** @type {[string, string][]} */
  const terminalData = await fetchOrigin(`/api/getTerminal?id=${num}`, {
    method: "GET"
  }).then(async res => await res.json());

  if (addNewWindowElm) {
    openWindowsElm.appendChild(genWindow(num));
  }
  contentElm.replaceChildren(...(genContent(terminalData)));
  terminalIndicatorElm.textContent = num;
  CUR_TERM = num;
  [...document.querySelectorAll(".window")].forEach(windowElm => windowElm.classList.remove("open"));
  document.querySelector(`div[data-terminal-id="${num}"]`).classList.add("open");
}

/**
 * @param {string} cmd 
 */
async function exec(cmd) {
  await fetchOrigin("/api/exec", {
    method: "POST",
    body: CUR_TERM + "\n" + cmd
  });
  await selectTerminal(CUR_TERM, false);
}