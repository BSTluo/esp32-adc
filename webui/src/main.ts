import { createApp } from 'vue';
import './style.css';
import App from './App.vue';
import router from './router/index';
import store from './store/index';
import 'bootstrap-icons/font/bootstrap-icons.css';

createApp(App).use(router).use(store).mount('#app');
