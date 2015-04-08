<?php
/* @var $this WSNkongyadataController */
/* @var $model WSNkongyadata */

$this->breadcrumbs=array(
	'Wsnkongyadatas'=>array('index'),
	'Create',
);

$this->menu=array(
	array('label'=>'List WSNkongyadata', 'url'=>array('index')),
	array('label'=>'Manage WSNkongyadata', 'url'=>array('admin')),
);
?>

<h1>Create WSNkongyadata</h1>

<?php $this->renderPartial('_form', array('model'=>$model)); ?>