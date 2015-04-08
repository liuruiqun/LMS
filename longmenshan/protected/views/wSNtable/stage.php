<?php
/**
 * Created by PhpStorm.
 * User: LiuRuiQun
 * Date: 15-1-28
 * Time: 下午3:47
 */

$this->breadcrumbs=array(
    '水位',
);

$this->menu=array(
    array('label'=>'Create stagedata', 'url'=>array('create')),
    array('label'=>'Manage stagedata', 'url'=>array('admin')),
);
?>

<h1>水位</h1>

<?php $this->widget('zii.widgets.CListView', array(
    'dataProvider'=>$dataProvider,
    'itemView'=>'_view',
)); ?>
