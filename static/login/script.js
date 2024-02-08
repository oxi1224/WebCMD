import { fetchOrigin } from "../util.js";

const loginForm = document.getElementById("loginForm");
const loginInput = document.getElementById("login");
const passwordInput = document.getElementById("password");

loginForm.addEventListener("submit", async (e) => {
  e.preventDefault();
  const login = loginInput.value;
  const password = passwordInput.value;
  if (!login || !password) return;
  const res = await fetchOrigin("/api/login", {
    method: "POST",
    cache: "no-cache",
    headers: {
      "Content-type": "text/plain" 
    },
    body: login + "\n" + password
  });
  if (!res.ok) {
    const resText = await res.text();
    alert(resText);
  } else {
    window.location.reload();
  }
});