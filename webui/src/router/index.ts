import { createRouter, createWebHashHistory } from 'vue-router';

const routes = [
  {
      path: '/',
      name: 'main',
      // route level code-splitting
      // this generates a separate chunk (about.[hash].js) for this route
      // which is lazy-loaded when the route is visited.
      component: () => import('../views/main/index.vue')
  }
];

const router = createRouter({
  history: createWebHashHistory(),
  routes
});

export default router


