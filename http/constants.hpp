#ifndef CONSTANT_HPP
#define CONSTANT_HPP

namespace http {
	namespace constants {
		const char* liveReloadInject = R""""(
<!-- Code Injected By Server -->
<script>
  const url = window.location.origin + "/dev/check-refresh";
  async function fetchStatus() {
    const res = await fetch(url);
    const text = await res.text();
    if (text == "true") {
      window.location.reload();
    }
  }

  (async () => {
    const url = window.location.origin + "/dev/check-refresh";
    try {
      await fetchStatus();
      setInterval(async () => await fetchStatus(), 1000);
    } catch (e) { }
  })();
</script>
)"""";
	}
}

#endif