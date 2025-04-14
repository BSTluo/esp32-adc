<script lang="ts" setup>

const iconList = [
  { classBase: 'bi bi-box-arrow-in-down', name: 'saveConfig' },
  { classBase: 'bi bi-box-arrow-in-up', name: 'readConfig' },
  { classBase: 'bi bi-play-fill', name: 'connectServer', classActivate: ' bi-stop-fill' },
  { classBase: 'bi bi-gear', name: 'ipConfig' },
  { classBase: 'bi bi-gear', name: 'portConfig' },
];

const clickFunctionbase = (event, name) =>
{
  const target = event.currentTarget as HTMLElement;

  // 创建 ripple 元素
  const ripple = document.createElement('span');
  ripple.className = 'ripple';

  const rect = target.getBoundingClientRect();
  const size = Math.max(rect.width, rect.height);
  ripple.style.width = ripple.style.height = `${size}px`;
  ripple.style.left = `${event.clientX - rect.left - size / 2}px`;
  ripple.style.top = `${event.clientY - rect.top - size / 2}px`;

  target.appendChild(ripple);

  // 动画结束后移除
  setTimeout(() =>
  {
    ripple.remove();
  }, 600);
};

const clickFunctionPack = {

};
</script>

<template>
  <div class="mainView">
    <div class="headView">
      <!-- 读取配置 -->
      <div class="iconItemBoxs" v-for="item in iconList" @click="clickFunctionbase($event, item.name)">
        <div class="icon" :class="item.classBase"></div>

      </div>

      <!-- 保存配置 -->
      <!-- 连接/断开 -->
      <!-- 目标IP -->
      <!-- 目标端口 -->
    </div>
    <div class="bodyView">
      <!-- 模拟量输入配置 -->
      <!-- 数字量输出配置 -->
    </div>
  </div>
</template>

<style scoped lang="scss">
.mainView {
  .headView {
    height: 5%;
    width: 100%;
    background-color: #323232;
    display: flex;
    align-items: center;
    color: white;

    .iconItemBoxs {
      position: relative; // 关键
      overflow: hidden; // 防止波纹超出
      margin-left: 40px;
      display: flex;
      justify-content: center;
      align-items: center;
      cursor: pointer;
      user-select: none;

      .icon {
        width: 30px;
        height: 30px;
        border: 1px solid white;
        border-radius: 5px;
        display: flex;
        justify-content: center;
        align-items: center;
        font-size: 20px;
        z-index: 2;
      }

      .ripple {
        position: absolute;
        border-radius: 50%;
        background-color: rgba(255, 255, 255, 0.5);
        transform: scale(0);
        animation: ripple 0.6s linear;
        pointer-events: none;
      }

      @keyframes ripple {
        to {
          transform: scale(4);
          opacity: 0;
        }
      }
    }


  }

  .bodyView {
    height: 95%;
    width: 100%;
  }
}
</style>

<style lang="scss">
.mainView .headView .iconItemBoxs {

  .ripple {
    position: absolute;
    border-radius: 50%;
    background-color: rgba(255, 255, 255, 0.5);
    transform: scale(0);
    animation: ripple 0.6s linear;
    pointer-events: none;
  }

  @keyframes ripple {
    to {
      transform: scale(4);
      opacity: 0;
    }
  }
}
</style>