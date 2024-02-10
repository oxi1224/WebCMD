/**
 * @param {number} num 
 * @returns HTMLDivElement
 */
export function genWindow(num) {
  const mainDiv = document.createElement("div");
  mainDiv.classList.add(...["window", "open"]);
  mainDiv.textContent = `Terminal ${num}`;
  const button = document.createElement("button");
  button.classList.add("control-btn");
  mainDiv.setAttribute("data-terminal-id", num);
  const svg = document.createElementNS("http://www.w3.org/2000/svg", "svg");
  const svgUse = document.createElementNS(
    "http://www.w3.org/2000/svg",
    "use"
  );
  svgUse.setAttribute("href", "./assets/svg/cross.svg#cross");
  svg.appendChild(svgUse);
  button.appendChild(svg);
  mainDiv.appendChild(button);
  return mainDiv;
}

/**
 * @param {[string, string][]} termData
 * @returns HTMLDivElement[]
 */
export function genContent(termData) {
  /** @type {HTMLDivElement[]} */
  const outArr = [];
  for (const [cmd, out] of termData.reverse()) {
    const inputDiv = document.createElement("div");
    inputDiv.classList.add(...["line", "input"]);
    inputDiv.textContent = "> " + cmd;
    outArr.push(inputDiv);
    const splitOut = out.trim().split("\n");
    for (let i = 0; i < splitOut.length; i++) {
      const lineDiv = document.createElement("div");
      lineDiv.classList.add("line");
      lineDiv.innerHTML = `<span class="tab">${(i === (splitOut.length - 1)) ? "└" : "├"}</span>` + splitOut[i];
      outArr.push(lineDiv);
    }
  }
  return outArr;
}

const baseUrl = window.location.origin;
/**
 * @param {string} path 
 * @param {RequestInit} requestInit 
 * @returns Promise<Response>
 */
export async function fetchOrigin(path, requestInit) {
  return fetch(baseUrl + path, requestInit); 
}