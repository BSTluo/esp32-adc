<script lang="ts" setup>
import { ref, type Ref } from 'vue';

const iconList = [
  { classBase: 'bi bi-box-arrow-in-down', name: 'saveConfig' },
  { classBase: 'bi bi-box-arrow-in-up', name: 'readConfig' },
  { classBase: 'bi bi-play-fill', name: 'connectServer', classActivate: ' bi-stop-fill' },
];

const inputList = [
  { classBase: 'bi bi-gear', name: 'ipConfig', title: 'IP配置：', value: 'ip' },
  { classBase: 'bi bi-gear', name: 'portConfig', title: '端口配置：', value: 'port' }
];

const config: Record<string, any> = {
  ip: "",
  port: "",
  inputList: [
    { name: '1', status: -1, value: 0 }, // status: -1禁用，0模拟量输入，1数字量输入，value:数字量值，max:模拟量上限，min：模拟量下限
    { name: '2', status: -1, value: 0 },
    { name: '3', status: -1, value: 0 },
    { name: '4', status: -1, value: 0 },
    { name: '5', status: -1, value: 0 },
    { name: '6', status: -1, value: 0 },
    { name: '7', status: -1, value: 0 },
    { name: '8', status: -1, value: 0 },
    { name: '9', status: -1, value: 0 },
    { name: '10', status: -1, value: 0 }
  ],
  outputList: [
    { name: '1', status: 0 }, // status: 0：数字量输出false，1：数字量输出true
    { name: '2', status: 0 },
    { name: '3', status: 0 },
    { name: '4', status: 0 },
    { name: '5', status: 0 },
    { name: '6', status: 0 },
    { name: '7', status: 0 },
    { name: '8', status: 0 },
    { name: '9', status: 0 },
    { name: '10', status: 0 },
  ]
};

const clickFunctionbase = (event: MouseEvent, name: string) =>
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

  clickFunctionPack[name]();
};

let connectServerStatus = false; // 连接状态

const clickFunctionPack: Record<string, () => void> = {
  saveConfig: () =>
  {
    console.log('保存配置');
  },
  readConfig: () =>
  {
    console.log('读取配置');
  },
  connectServer: () =>
  {
    console.log('连接服务器');
    console.log(config.inputList);
  },
};

const getInputWidth = () =>
{
  const max = config.inputList.length + config.outputList.length;
  const min = config.inputList.length + 2;
  const width = 98 * (min / max);
  return width;
};

const getOutputWidth = () =>
{
  const max = config.inputList.length + config.outputList.length;
  const min = config.outputList.length - 2;
  const width = 98 * (min / max);
  return width;
};
</script>

<template>
  <div class="mainView">
    <div class="headView">
      <!-- 读取配置 -->
      <div class="iconItemBoxs" v-for="item in iconList" @click="clickFunctionbase($event, item.name)">
        <div class="icon" :class="item.classBase"></div>

      </div>

      <div class="inputItemBox" v-for="item in inputList">
        <div class="title">{{ item.title }}</div>
        <input class="inputBox" v-model="config[item.value]"></input>
      </div>
      <!-- 目标IP -->
      <!-- 目标端口 -->
    </div>
    <div class="bodyViewBox">
      <div class="bodyView">
        <div class="splitBox"></div>
        <div class="mainViewBox">

          <!-- 状态栏 -->
          <div class="statusListBox">
            <!-- 行 -->
            <div class="rowItem"></div>
            <div class="rowItem">通道</div>
            <div class="rowItem">实时</div>
          </div>

          <!-- 模拟量输入配置 -->
          <div class="inputViewListBox" :style="{ width: `${getInputWidth()}%` }">
            <!-- 行 -->
            <div class="rowItem">
              输入配置
            </div>

            <div class="rowItem">
              <!-- 列 -->
              <div class="columnItem" :style="{ width: `${100 / config.inputList.length}%` }"
                v-for="item of config.inputList">
                <div class="itemTitle">{{ item.name }}</div>
                <div class="itemConfig">
                  <select v-model.number="item.status">
                    <option :value="-1">禁用</option>
                    <option :value="0">模拟量</option>
                    <option :value="1">数字量</option>
                  </select>
                </div>
              </div>
            </div>

            <div class="rowItem">
              <!-- 列 -->
              <div class="columnItem" :style="{ width: `${100 / config.inputList.length}%` }"
                v-for="item of config.inputList"></div>
            </div>
          </div>

          <!-- 数字量输出配置 -->
          <div class="outputViewListBox" :style="{ width: `${getOutputWidth()}%` }">
            <div class="rowItem">
              输出配置
            </div>

            <div class="rowItem">
              <!-- 列 -->
              <div class="columnItem" :style="{ width: `${100 / config.outputList.length}%` }"
                v-for="item of config.inputList">{{ item.name }}</div>
            </div>

            <div class="rowItem">
              <!-- 列 -->
              <div class="columnItem" :style="{ width: `${100 / config.outputList.length}%` }"
                v-for="item of config.inputList"></div>
            </div>
          </div>
        </div>
        <div class="splitBox"></div>
      </div>
    </div>
  </div>
</template>

<style scoped lang="scss">
.mainView {
  --mainFontColor: #8E8E8E;

  .headView {
    height: 5%;
    width: 100%;
    background-color: #323233;
    display: flex;
    align-items: center;
    color: var(--mainFontColor);

    .iconItemBoxs {
      position: relative; // 关键
      overflow: hidden; // 防止波纹超出
      margin-left: 40px;
      display: flex;
      justify-content: center;
      align-items: center;
      cursor: pointer;
      user-select: none;
      background-color: #252526;

      .icon {
        width: 25px;
        height: 25px;
        border: 1px solid var(--mainFontColor);
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

    .inputItemBox {
      margin-left: 40px;
      display: flex;
      justify-content: center;
      align-items: center;
      cursor: pointer;
      user-select: none;

      .title {
        font-size: 20px;
      }

      .inputBox {
        border-radius: 5px;
        border: 1px solid var(--mainFontColor);
        background-color: #787878;
        font-size: 20px;
      }
    }
  }

  .bodyViewBox {
    height: 95%;
    width: 100%;
    background-color: #1E1E1E;
    display: flex;
    align-items: center;
    justify-content: center;

    .bodyView {
      height: 100%;
      width: 95%;
      overflow-y: auto;
      scrollbar-width: none;
      -ms-overflow-style: none;

      .splitBox {
        height: 5%;
        width: 100%;
      }

      .splitBox::-webkit-scrollbar {
        display: none;
        /* Chrome、Safari 隐藏滚动条 */
      }

      .mainViewBox {
        height: 90%;
        width: 100%;
        background-color: #323233;
        display: flex;
        align-items: center;


        .statusListBox {
          width: 2%;
          height: 100%;
          border-right: 1px solid #4a4a4a;

          .rowItem {
            height: 75px;
            width: 100%;
            border-bottom: 1px solid #4a4a4a;
            display: flex;
            align-items: center;
            justify-content: center;
            flex-direction: row;
            color: var(--mainFontColor);
          }
        }

        .inputViewListBox {
          height: 100%;
          border-right: 1px solid #4a4a4a;

          .rowItem {
            height: 75px;
            width: 100%;
            border-bottom: 1px solid #4a4a4a;
            display: flex;
            align-items: center;
            flex-direction: row;
            justify-content: center;
            color: var(--mainFontColor);

            .columnItem {
              height: 100%;
              border-right: 1px solid #4a4a4a;
              display: flex;
              align-items: center;
              justify-content: center;

              .itemTitle {
                height: 100%;
                width: 30%;
                display: flex;
                align-items: center;
                justify-content: center;
              }

              .itemConfig {
                height: 100%;
                width: 70%;
                display: flex;
                align-items: center;
                justify-content: center;
              }
            }
          }
        }

        .outputViewListBox {
          height: 100%;

          .rowItem {
            height: 75px;
            width: 100%;
            border-bottom: 1px solid #4a4a4a;
            display: flex;
            align-items: center;
            flex-direction: row;
            justify-content: center;
            color: var(--mainFontColor);

            .columnItem {
              height: 100%;
              border-right: 1px solid #4a4a4a;
              display: flex;
              align-items: center;
              justify-content: center;
            }
          }
        }
      }
    }
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