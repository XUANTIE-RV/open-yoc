export default {
  computed: {
    isLightTheme() {
      return ['', 'theme-light'].includes(this.theme)
    }
  },
  data() {
    return {
      theme: 'theme-dark',
    }
  },
  mounted() {
    this.onChangeThemeFunc = (...args) => this.onChangeTheme(...args)
    this.$page.on("show", this.onBindTheme)
    this.$page.on("hide", this.onUnbindTheme)
  },
  methods: {
    onChangeTheme(theme) {
      this.theme = theme.data.n
    },
    onBindTheme() {
      $falcon.on('system_env_custom_theme', this.onChangeThemeFunc)
    },
    onUnbindTheme() {
      $falcon.off('system_env_custom_theme', this.onChangeThemeFunc)
    }
  },
}